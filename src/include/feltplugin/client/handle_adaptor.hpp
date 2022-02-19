#pragma once

#include <stdexcept>

#include <feltplugin/interface.h>
#include <feltplugin/error_map.hpp>

namespace feltplugin::client
{

template <class THandle, class THandleMap, class TErrorMap = DefaultErrorMap>
struct HandleAdapter
{
protected:
	using Base = HandleAdapter<THandle, THandleMap, TErrorMap>;

public:
	using Handle = THandle;
	using Suite = typename THandleMap::template suite_from_handle<THandle>;
	using SuiteFactory = Suite (*)();

protected:
	static constexpr SuiteFactory ksuite_factory =
		THandleMap::template suite_factory_from_handle<THandle>();

public:
	HandleAdapter(Handle handle)  // NOLINT(google-explicit-constructor)
		: HandleAdapter{handle, ksuite_factory}
	{
		static_assert(ksuite_factory != nullptr, "Attempting to construct with null suite factory");
	}

	explicit HandleAdapter(SuiteFactory suite_factory) : HandleAdapter{nullptr, suite_factory} {}

	explicit HandleAdapter(Handle handle, SuiteFactory suite_factory)
		: handle_{handle}, suite_{suite_factory()}
	{
	}

	HandleAdapter(HandleAdapter const &) = delete;
	HandleAdapter(HandleAdapter && other) noexcept : handle_{other.handle_}, suite_{other.suite_}
	{
		other.handle_ = nullptr;
	};

	virtual ~HandleAdapter()
	{
		if (handle_ == nullptr)
			return;	 // Assume moved out

		// E.g. a handle to a temporary shouldn't have a `release` function in its suite (though
		// technically it could but should be a no-op, and certainly not try to free memory pointed
		// to by the handle)
		if constexpr (has_release_t<Suite>::value)
			suite_.release(handle_);

		handle_ = nullptr;
	}

	explicit operator Handle() const
	{
		return handle_;
	}

protected:
	HandleAdapter() : HandleAdapter{Handle{nullptr}} {}

	template <class... Args>
	void create(Args &&... args)
	{
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = suite_.create(err, &handle_, std::forward<Args>(args)...);
		throw_on_error(code, err);
	}

	template <class Ret, class... Args, class... Rest>
	Ret call(fp_ErrorCode (*fn)(fp_ErrorMessage, Ret *, Handle, Args...), Rest &&... args) const
	{
		Ret ret;
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = fn(err, &ret, handle_, std::forward<Rest>(args)...);
		throw_on_error(code, err);
		return ret;
	}

	template <class... Args, class... Rest>
	void call(fp_ErrorCode (*fn)(fp_ErrorMessage, Handle, Args...), Rest &&... args) const
	{
		fp_ErrorCode code = 1;
		fp_ErrorMessage err;

		code = fn(err, handle_, std::forward<Rest>(args)...);
		throw_on_error(code, err);
	}

	static void throw_on_error(fp_ErrorCode const code, fp_ErrorMessage const & err)
	{
		if (code == fp_ok)
			return;

		TErrorMap::exception_from_code(code, err);
	}

protected:
	Handle handle_;
	Suite const suite_;

private:
	template <typename T, typename = void>
	struct has_release_t : std::false_type
	{
	};

	template <typename T>
	struct has_release_t<T, decltype(T::release, void())> : std::true_type
	{
	};
};
}  // namespace feltplugin::client