// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the HandleAdapter to be used by clients wrapping service types.
 */
#pragma once

#include <stdexcept>

#include <feltpluginsystem/interface.h>
#include <feltpluginsystem/error_map.hpp>

namespace feltplugin::client
{
/**
 * Base class for adapters wrapping opaque handles on the client.
 *
 * @tparam THandle Opaque handle type.
 * @tparam THandleMap HandleMap detailing mapping of handles to wrapper classes.
 * @tparam TErrorMap ErrorMap detailing mapping of exceptions to error codes.
 */
template <class THandle, class THandleMap, class TErrorMap>
struct SuiteAdapter
{
	static constexpr std::size_t default_error_capacity = 500;

protected:
	/// Convenience for referring to this base class in subclasses.
	using Base = SuiteAdapter<THandle, THandleMap, TErrorMap>;

public:
	/// Opaque handle type.
	using Handle = THandle;
	/// C function pointer suite associated with the handle.
	using Suite = typename THandleMap::template suite_from_handle<THandle>;
	/// Signature of function used to construct a function pointer suite for the handle.
	using SuiteFactory = Suite (*)();

protected:
	/**
	 * Compile-time known factory for creating function pointer suite associated with handle.
	 *
	 * Can be `nullptr` (the default), in which case the suite factory must be passed to the
	 * constructor.
	 */
	static constexpr SuiteFactory ksuite_factory =
		THandleMap::template suite_factory_from_handle<THandle>();

public:
	/**
	 * Construct from a given handle, assuming compile-time known associated function pointer suite.
	 *
	 * Typically this constructor is used for plugin clients wrapping host service types, where the
	 * host DSO has already exported the suite factory as a global symbol available immediately on
	 * loading (linking) the plugin.
	 *
	 * @param handle Opaque handle to service type.
	 */
	SuiteAdapter(Handle handle)  // NOLINT(google-explicit-constructor)
		: SuiteAdapter{ksuite_factory, handle}
	{
		static_assert(ksuite_factory != nullptr, "Attempting to construct with null suite factory");
	}

	/**
	 * Construct injecting provided function pointer suite, initially wrapping a null handle.
	 *
	 * Typically this constructor is used by host clients wrapping plugin service types, where the
	 * suite factory has to be queried from the plugin DSO. Once this constructor has been called,
	 * the subclass should immediately use the suite's `create` function to fill in the null handle.
	 *
	 * @param suite_factory Factory function that returns the function pointer suite associated with
	 * the handle.
	 */
	explicit SuiteAdapter(SuiteFactory suite_factory) : SuiteAdapter{suite_factory, nullptr} {}

	/**
	 * Construct injecting provided opaque handle and associated function pointer suite.
	 *
	 * @param handle Opaque handle to service type.
	 * @param suite_factory Factory function that returns the function pointer suite associated with
	 * the handle.
	 */
	explicit SuiteAdapter(SuiteFactory suite_factory, Handle handle)
		: suite_{suite_factory()}, handle_{handle}
	{
	}

	/// It is not safe to copy a handle adapter, since it may lead to use-after-free.
	SuiteAdapter(SuiteAdapter const &) = delete;

	/// Move the handle from the other adapter and set its handle to null.
	SuiteAdapter(SuiteAdapter && other) noexcept : suite_{other.suite_}, handle_{other.handle_}
	{
		other.handle_ = nullptr;
	};

	/**
	 * Call our suite's `release` function, if appropriate.
	 *
	 * I.e. Call `release` if the handle is not null and the suite defines a `release` function.
	 */
	virtual ~SuiteAdapter()
	{
		if (handle_ == nullptr)
			return;	 // Assume moved out

		// E.g. an OwnedByService handle shouldn't have a `release` function in its suite (though
		// technically it could but should be a no-op, and certainly not try to free memory pointed
		// to by the handle)
		if constexpr (has_release_t<Suite>::value)
			suite_.release(handle_);

		handle_ = nullptr;
	}

	/// Allow `static_cast`ing from this adapter back to a raw handle.
	explicit operator Handle() const
	{
		return handle_;
	}

protected:
	/// Allow default construction, relying on the subclass to populate the handle.
	SuiteAdapter() : SuiteAdapter{Handle{nullptr}} {}

	/**
	 * Call our suite's `create` function, updating our opaque handle with the result.
	 *
	 * Assumes `create` is defined in the function pointer suite with signature
	 * `(fp_ErrorMessage*, Handle*, Args...) -> fp_ErrorCode`.
	 *
	 * No conversion to opaque handles is performed - if these are required by the `create` function
	 * then conversion must happen in the caller.
	 *
	 * @warning This must not be called until _after_ the constructor, otherwise the function
	 * pointer suite will be in an uninitialized state.
	 *
	 * @tparam Args Additional constructor argument types.
	 * @param args Constructor arguments.
	 */
	template <class... Args>
	void create(Args &&... args)
	{
		if (handle_ != nullptr)
			throw std::invalid_argument{
				"Cannot `create` a handle adapter if handle is already assigned."};
		// TODO: suite_.create is not default initialized (to nullptr).
		//		if (suite_.create == nullptr)
		//			throw std::invalid_argument{
		//				"Cannot `create` a handle adapter when no function pointer suite is
		// assigned."};
		fp_ErrorCode code;
		fp_ErrorMessage err{err_storage_.size(), 0, err_storage_.data()};

		code = suite_.create(&err, &handle_, std::forward<Args>(args)...);
		throw_on_error(code, err);
	}

	/**
	 * Call a suite function that has a return value and can error.
	 *
	 * The handle, error message/code, and return value out parameter are all handled, allowing the
	 * caller to just provide any additional arguments specific to the given function.
	 *
	 * A non-zero error code is thrown as an exception, as defined by the ErrorMap.
	 *
	 * @tparam Ret Type of return value (out parameter).
	 * @tparam Args Additional argument types required by the suite function.
	 * @tparam Rest Additional argument types given to the suite function.
	 * @param fn Suite function to call.
	 * @param args Additional arguments given to the suite function.
	 * @return Value of suite function's out parameter after invocation.
	 */
	template <class Ret, class... Args, class... Rest>
	Ret call(fp_ErrorCode (*fn)(fp_ErrorMessage *, Ret *, Handle, Args...), Rest &&... args) const
	{
		Ret ret;
		fp_ErrorCode code;
		fp_ErrorMessage err{err_storage_.size(), 0, err_storage_.data()};

		code = fn(&err, &ret, handle_, std::forward<Rest>(args)...);
		throw_on_error(code, err);
		return ret;
	}

	/**
	 * Call a suite function that has no return value but can error.
	 *
	 * The handle and error message/code all handled, allowing the caller to just provide any
	 * additional arguments specific to the given function.
	 *
	 * A non-zero error code is thrown as an exception, as defined by the ErrorMap.
	 *
	 * @tparam Args Additional argument types required by the suite function.
	 * @tparam Rest Additional argument types given to the suite function.
	 * @param fn Suite function to call.
	 * @param args Additional arguments given to the suite function.
	 */
	template <class... Args, class... Rest>
	void call(fp_ErrorCode (*fn)(fp_ErrorMessage *, Handle, Args...), Rest &&... args) const
	{
		fp_ErrorCode code;
		fp_ErrorMessage err{err_storage_.size(), 0, err_storage_.data()};

		code = fn(&err, handle_, std::forward<Rest>(args)...);
		throw_on_error(code, err);
	}

	/**
	 * Call a suite function that has no return value and cannot error.
	 *
	 * The handle is injected, allowing the caller to just provide any
	 * additional arguments specific to the given function.
	 *
	 * @tparam Args Additional argument types required by the suite function.
	 * @tparam Rest Additional argument types given to the suite function.
	 * @param fn Suite function to call.
	 * @param args Additional arguments given to the suite function.
	 */
	template <class... Args, class... Rest>
	void call(void (*fn)(Handle, Args...), Rest &&... args) const
	{
		fn(handle_, std::forward<Rest>(args)...);
	}

	/**
	 * Call a suite function that has arguments nor return value and cannot error.
	 *
	 * The handle is injected.
	 *
	 * @tparam Args Additional argument types required by the suite function.
	 * @tparam Rest Additional argument types given to the suite function.
	 * @param fn Suite function to call.
	 * @param args Additional arguments given to the suite function.
	 */
	void call(void (*fn)(Handle)) const
	{
		fn(handle_);
	}

	/**
	 * Convert an error code into an exception and throw it.
	 *
	 * @param code Code to look up.
	 * @param err Error message to pass to constructor of exception.
	 */
	static void throw_on_error(fp_ErrorCode const code, fp_ErrorMessage const & err)
	{
		if (code == fp_ok)
			return;

		TErrorMap::throw_exception(err, code);
	}

protected:
	/// Function pointer suite associated with Handle.
	Suite const suite_;
	/// Opaque handle to C++ object in the service.
	Handle handle_;

	/// Storage for error messages.
	inline static thread_local auto err_storage_ = std::string(default_error_capacity, '\0');

private:
	/**
	 * Check if given type has a `release` member.
	 *
	 * SFINAE type. This default means the given type has no `release` member.
	 *
	 * @tparam T Type to check.
	 */
	template <typename T, typename = void>
	struct has_release_t : std::false_type
	{
	};

	/**
	 * Check if given type has a `release` member.
	 *
	 * SFINAE type. If this specialisation is chosen then the given type has a `release` member.
	 *
	 * @tparam T Type to check.
	 */
	template <typename T>
	struct has_release_t<T, decltype(T::release, void())> : std::true_type
	{
	};
};
}  // namespace feltplugin::client