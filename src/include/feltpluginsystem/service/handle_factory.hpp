// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the HandleConverter and SuiteDecorator to be used by services converting to/from opaque
 * handles and constructing function pointer suites..
 */
#pragma once

#include <cstring>
#include <functional>

#include "../error_map.hpp"
#include "../interface.h"
#include "../pointers.hpp"
#include "handle_map.hpp"

namespace feltplugin::service
{

/**
 * Utility to static_assert that a given handle maps to either a native class or adapter.
 *
 * If the Handle type is not found in the service::HandleMap, then Class resolves to Handle
 * (pass-through). If the Handle type is not found in the client::HandleMap, then the
 * Adapter resolves to `std::false_type`.
 *
 * @tparam Handle Handle type to check.
 * @tparam Class Native class type that the handle is associated with, if any.
 * @tparam Adapter Adapter class that the handle is associated with, if any.
 */
template <class Handle, class Class, class Adapter>
struct assert_is_valid_handle_type
{
	static_assert(
		!(std::is_same_v<Handle, Class> && std::is_same_v<Adapter, std::false_type>),
		"Attempting to wrap a handle that is unrecognized. Are you missing an entry in your "
		"HandleMap lists?");
};

/**
 * Utility class for creating and converting opaque handles.
 *
 * @tparam THandle Opaque handle type.
 * @tparam TServiceHandleMap service::HandleMap for mapping handles to native classes.
 * @tparam TClientHandleMap client::HandleMap for mapping handles to client adapter classes.
 * @tparam TErrorMap ErrorMap for mapping exceptions to error codes.
 */
template <
	class THandle,
	class TServiceHandleMap,
	class TClientHandleMap,
	class TErrorMap = ErrorMap<>>
struct HandleManager
{
private:
	using Handle = THandle;
	using Class = typename TServiceHandleMap::template class_from_handle<Handle>;
	using Adapter = typename TClientHandleMap::template class_from_handle<Handle>;
	static constexpr HandleOwnershipTag ptr_type_tag =
		TServiceHandleMap::template ownersihp_tag_from_handle<Handle>();

public:
	/**
	 * Convert an opaque handle to a concrete instance.
	 *
	 * The behaviour of this function varies depending on `ptr_type_tag` in our HandleTraits.
	 * However, a dereferencable object is always returned, which when dereferenced yields the
	 * concrete object. This can be either the original object that is associated with the handle,
	 * or a client adapter class that wraps the handle.
	 *
	 * @tparam Handle Type of handle. Required to enable forwarding references.
	 * @param handle Opaque handle to convert.
	 * @return Dereferenceable object that dereferences to the original object associated with the
	 * opaque handle.
	 */
	template <class Handle>
	static decltype(auto) convert(Handle && handle)
	{
		if constexpr (
			ptr_type_tag == HandleOwnershipTag::OwnedByClient ||
			ptr_type_tag == HandleOwnershipTag::OwnedByService)
		{
			return reinterpret_cast<Class *>(handle);
		}
		else if constexpr (ptr_type_tag == HandleOwnershipTag::Shared)
		{
			return *reinterpret_cast<SharedPtr<Class> *>(handle);
		}
		else if constexpr (ptr_type_tag == HandleOwnershipTag::Unrecognized)
		{
			if constexpr (std::is_same_v<Adapter, std::false_type>)
			{
				// Native C type.
				return &handle;
			}
			else
			{
				// Client handle type. Create adapter object and return a dereferencable object that
				// resolves to the adapter. Messy, but necessary to be consistent with other code
				// paths.
				return Dereferenceable<Adapter>{Adapter{handle}};
			}
		}
	}

	/**
	 * Construct a new instance of our Class type and associate it with a Handle.
	 *
	 * Ownership is determined by the `ptr_type_tag` enum value in the HandleTraits for our Handle.
	 *
	 * This function is not valid if the HandlePtrTag is `OwnedByService`, since that implies
	 * a handle should be associated with an existing object rather than creating a new one.
	 *
	 * @tparam Args Argument types to pass to the constructor.
	 * @param args Arguments to pass to the constructor.
	 * @return Newly minted opaque handle.
	 */
	template <typename... Args>
	static Handle make_handle(Args &&... args)
	{
		static_assert(
			ptr_type_tag != HandleOwnershipTag::OwnedByService,
			"Cannot make a handle to a new instance for non-shared non-transferred types");

		if constexpr (ptr_type_tag == HandleOwnershipTag::Shared)
		{
			return create(feltplugin::make_shared<Class>(std::forward<Args>(args)...));
		}
		else if constexpr (ptr_type_tag == HandleOwnershipTag::OwnedByClient)
		{
			return reinterpret_cast<Handle>(new Class{std::forward<Args>(args)...});
		}
		// Native type.
		else if constexpr (ptr_type_tag == HandleOwnershipTag::Unrecognized)
		{
			static_assert(
				std::is_same_v<Adapter, std::false_type>,
				"Reconstructing a handle when it should be wrapped.");
			return Class{std::forward<Args>(args)...};
		}
	}
	/**
	 * Create a handle associated with a pre-existing instance.
	 *
	 * This function is only valid if the HandlePtrTag in the HandleTraits is `OwnedByService`.
	 *
	 * @param obj Object to reference.
	 * @return Newly minted opaque handle.
	 */
	static Handle create(Class & obj)
	{
		(void)assert_is_valid_handle_type<Handle, Class, Adapter>{};
		static_assert(
			ptr_type_tag == HandleOwnershipTag::OwnedByService,
			"Cannot create a non-shared non-transferred handle for shared / transferred types");
		return reinterpret_cast<Handle>(&obj);
	}

	/**
	 * Create a handle associated with a pre-existing shared smart pointer to an instance.
	 *
	 * This function is only valid if the `ptr_type_tag` in the HandleTraits is `Shared`.
	 *
	 * The given shared pointer's reference count will be incremented and not decremented again
	 * until `release` is called.
	 *
	 * @param ptr Pointer to wrap.
	 * @return Newly minted opaque handle.
	 */
	static Handle create(SharedPtr<Class> const & ptr)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>{};
		static_assert(
			ptr_type_tag == HandleOwnershipTag::Shared,
			"Cannot create a shared handle for a non-shared type");
		return reinterpret_cast<Handle>(new SharedPtr<Class>{ptr});
	}

	/**
	 * Create a handle associated with a pre-existing shared smart pointer to an instance.
	 *
	 * This function is only valid if the `ptr_type_tag` in the HandleTraits is `Shared`.
	 *
	 * The given shared pointer's reference count will be incremented and not decremented again
	 * until `release` is called.
	 *
	 * @param ptr Pointer to wrap.
	 * @return Newly minted opaque handle.
	 */
	static Handle create(SharedPtr<Class> && ptr)
	{
		(void)assert_is_valid_handle_type<Handle, Class, Adapter>{};
		static_assert(
			ptr_type_tag == HandleOwnershipTag::Shared,
			"Cannot create a shared handle for a non-shared type");
		return reinterpret_cast<Handle>(new SharedPtr<Class>{ptr});
	}

	/**
	 * Construct a new instance of our Class type and associate it with a Handle.
	 *
	 * The signature of this function matches the convention that function pointer suites should
	 * adhere to, allowing it to be used directly without wrapping, e.g. when defining the `create`
	 * member during construction of a function pointer suite we can simply do
	 * `.create = &Converter::make_handle,`.
	 *
	 * @tparam Args Argument types to pass to the constructor.
	 * @param[out] err Storage for exception message, if one occurs during construction.
	 * @param[out] out Pointer to handle to newly constructed object.
	 * @param args Arguments to pass to the constructor.
	 * @return Error code.
	 */
	template <typename... Args>
	static fp_ErrorCode make(fp_ErrorMessage * err, Handle * out, Args... args)
	{
		(void)assert_is_valid_handle_type<Handle, Class, Adapter>{};
		return TErrorMap::wrap_exception(*err, [&out, &args...] { *out = make_handle(args...); });
	}

	/**
	 * Release an opaque handle.
	 *
	 * This function is only valid if the `ptr_type_tag` in our HandleTraits is `OwnedByClient` or
	 * `Shared`.
	 *
	 * If `OwnedByClient` then the object is destroyed. If `Shared` then the reference count is
	 * decremented, potentially destroying the object.
	 *
	 * @param handle Handle to release.
	 */
	static void release(Handle handle)
	{
		(void)assert_is_valid_handle_type<Handle, Class, Adapter>{};
		static_assert(
			ptr_type_tag != HandleOwnershipTag::OwnedByService,
			"Cannot release a handle not owned by client");
		static_assert(
			ptr_type_tag != HandleOwnershipTag::Unrecognized,
			"Cannot release a handle aliasing a temporary. Are you missing an entry in your "
			"HandleMaps?");

		if constexpr (ptr_type_tag == HandleOwnershipTag::Shared)
		{
			delete reinterpret_cast<SharedPtr<Class> *>(handle);
		}
		else if constexpr (ptr_type_tag == HandleOwnershipTag::OwnedByClient)
		{
			delete reinterpret_cast<Class *>(handle);
		}
	}

private:
	template <class T>
	struct Dereferenceable
	{
		Dereferenceable(Dereferenceable const &) = delete;
		T t;
		T operator*() &&
		{
			return std::move(t);
		}
	};
};

/**
 * Utility class for decorating callables to be used in function pointer suites.
 *
 * @tparam THandle Opaque handle type.
 * @tparam TServiceHandleMap service::HandleMap for mapping handles to native classes.
 * @tparam TClientHandleMap client::HandleMap for mapping handles to client adapter classes.
 * @tparam TErrorMap ErrorMap for mapping exceptions to error codes.
 */
template <
	class THandle,
	class TServiceHandleMap,
	class TClientHandleMap,
	class TErrorMap = ErrorMap<>>
struct SuiteDecorator
{
private:
	template <class Handle>
	using HandleManager = HandleManager<Handle, TServiceHandleMap, TClientHandleMap, TErrorMap>;

	using Handle = THandle;
	using Class = typename TServiceHandleMap::template class_from_handle<Handle>;
	using Adapter = typename TClientHandleMap::template class_from_handle<Handle>;
	static constexpr HandleOwnershipTag ptr_type_tag =
		TServiceHandleMap::template ownersihp_tag_from_handle<Handle>();

	static_assert(
		std::is_same_v<Handle, Class> | std::is_same_v<Adapter, std::false_type>,
		"Cannot have a handle that is associated with both a native type and a client adapter. "
		"Check HandleMap lists.");

	template <auto fn>
	struct mem_fn_ptr_t : std::integral_constant<decltype(fn), fn>
	{
		static_assert(
			std::is_member_function_pointer_v<decltype(fn)>,
			"mem_fn_ptr must only be used with member function pointers");
	};

public:
	template <auto fn>
	static constexpr mem_fn_ptr_t<fn> mem_fn_ptr{};

	/**
	 * Adapt a suite function to have a more C++-like interface, automatically converting
	 * handles.
	 *
	 * The arguments will be converted from handles to objects using `convert`, and the return
	 * value converted from an object to a handle using `make_handle`, as appropriate.
	 *
	 * @tparam Ret Return type of suite function.
	 * @tparam Fn Type of wrapped callable.
	 * @tparam Args Argument types to convert then pass to wrapped callable.
	 * @param fn Wrapped callable to execute.
	 * @param[out] err Storage for exception message, if any.
	 * @param[out] out Return value destination.
	 * @param handle Opaque handle to self.
	 * @param args Arguments to pass along to wrapped callable, converting from opaque handles
	 * to concrete types if necessary.
	 * @return Error code.
	 */
	template <typename Lambda>
	static auto decorate(Lambda && lambda)
	{
		(void)assert_is_valid_handle_type<Handle, Class, Adapter>{};
		static_assert(
			std::is_empty_v<Lambda>,
			"Stateful callables (e.g. capturing lambdas) are not supported");

		static const Lambda fn = std::forward<Lambda>(lambda);

		return [](auto... args)
		{
			static constexpr out_param_sig sig_type = suite_func_sig_type<decltype(args)...>();
			static_assert(sig_type != out_param_sig::unrecognised, "Ill-formed C suite function");

			if constexpr (sig_type == out_param_sig::cannot_return_cannot_error)
			{
				return [](Handle handle, auto... args)
				{
					const auto do_call = [&]
					{
						return fn(
							*HandleManager<Handle>::convert(handle),
							*HandleManager<decltype(args)>::convert(
								std::forward<decltype(args)>(args))...);
					};
					using Ret = decltype(do_call());

					// The `cannot_return_cannot_error` suite type refers to out-parameters. A suite
					// function that cannot error is free to use its return value for something
					// other than an error code. So detect if the wrapped function returns a value
					// and if so attempt to return it from the C function.
					if constexpr (std::is_void_v<Ret>)
					{
						do_call();
					}
					else
					{
						return HandleManager<Ret>::make_handle(do_call());
					}
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::cannot_return_can_error)
			{
				return [](fp_ErrorMessage * err, Handle handle, auto... args)
				{
					const auto do_call = [&]
					{
						return fn(
							*HandleManager<Handle>::convert(handle),
							*HandleManager<decltype(args)>::convert(
								std::forward<decltype(args)>(args))...);
					};

					// C suite function supporting error cases must use the return value for an
					// error code. In this `cannot_return_can_error` case there is no out-parameter
					// for returning a value through. So the wrapped C++ function should in turn not
					// return any value.
					static_assert(
						std::is_void_v<decltype(do_call())>,
						"Suite function signature does not support a return value");

					return TErrorMap::wrap_exception(*err, do_call);
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_return_cannot_error)
			{
				return [](auto * out, Handle handle, auto... args)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					auto const ret =
						fn(*HandleManager<Handle>::convert(handle),
						   *HandleManager<decltype(args)>::convert(
							   std::forward<decltype(args)>(args))...);

					*out = HandleManager<Out>::make_handle(ret);
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_return_can_error)
			{
				return [](fp_ErrorMessage * err, auto * out, Handle handle, auto... args)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					return TErrorMap::wrap_exception(
						*err,
						[handle, &out, &args...]
						{
							auto const ret =
								fn(*HandleManager<Handle>::convert(handle),
								   *HandleManager<decltype(args)>::convert(
									   std::forward<decltype(args)>(args))...);

							*out = HandleManager<Out>::make_handle(ret);
						});
				}(std::forward<decltype(args)>(args)...);
			}
		};
	}

	/**
	 * Adapt a suite function to have a more C++-like interface, automatically converting
	 * handles.
	 *
	 * The arguments will be converted from handles to objects using `convert`, and the return
	 * value converted from an object to a handle using `make_handle`, as appropriate.
	 *
	 * @tparam Ret Return type of suite function.
	 * @tparam Fn Type of wrapped callable.
	 * @tparam Args Argument types to convert then pass to wrapped callable.
	 * @param fn Wrapped callable to execute.
	 * @param[out] err Storage for exception message, if any.
	 * @param[out] out Return value destination.
	 * @param handle Opaque handle to self.
	 * @param args Arguments to pass along to wrapped callable, converting from opaque handles
	 * to concrete types if necessary.
	 * @return Error code.
	 */
	template <auto fn>
	static auto decorate(mem_fn_ptr_t<fn>)
	{
		(void)assert_is_valid_handle_type<Handle, Class, Adapter>{};

		return [](auto... args)
		{
			static constexpr out_param_sig sig_type = suite_func_sig_type<decltype(args)...>();
			static_assert(sig_type != out_param_sig::unrecognised, "Ill-formed C suite function");

			if constexpr (sig_type == out_param_sig::cannot_return_cannot_error)
			{
				return [](Handle handle, auto... args)
				{
					const auto do_call = [&]
					{
						return std::mem_fn(fn)(
							*HandleManager<Handle>::convert(handle),
							*HandleManager<decltype(args)>::convert(
								std::forward<decltype(args)>(args))...);
					};
					using Ret = decltype(do_call());

					// Although `cannot_return_cannot_error`, this refers to out-parameter, the
					// function may still return a value, so we must handle both cases.
					if constexpr (std::is_void_v<Ret>)
					{
						do_call();
					}
					else
					{
						return HandleManager<Ret>::make_handle(do_call());
					}
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::cannot_return_can_error)
			{
				return [](fp_ErrorMessage * err, Handle handle, auto... args)
				{
					const auto do_call = [&]
					{
						return std::mem_fn(fn)(
							*HandleManager<Handle>::convert(handle),
							*HandleManager<decltype(args)>::convert(
								std::forward<decltype(args)>(args))...);
					};
					return TErrorMap::wrap_exception(
						*err,
						[&do_call]
						{
							using Ret = decltype(do_call());

							// Although `cannot_return_can_error`, this refers to out-parameter,
							// the function may still return a value, so we must handle both cases.
							if constexpr (std::is_void_v<Ret>)
							{
								do_call();
							}
							else
							{
								return HandleManager<Ret>::make_handle(do_call());
							}
						});
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_return_cannot_error)
			{
				return [](auto * out, Handle handle, auto... args)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					auto const ret = std::mem_fn(fn)(
						*HandleManager<Handle>::convert(handle),
						*HandleManager<decltype(args)>::convert(
							std::forward<decltype(args)>(args))...);

					*out = HandleManager<Out>::make_handle(ret);
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_return_can_error)
			{
				return [](fp_ErrorMessage * err, auto out, Handle handle, auto... args)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					return TErrorMap::wrap_exception(
						*err,
						[handle, &out, &args...]
						{
							auto const ret = std::mem_fn(fn)(
								*HandleManager<Handle>::convert(handle),
								*HandleManager<decltype(args)>::convert(
									std::forward<decltype(args)>(args))...);

							*out = HandleManager<Out>::make_handle(ret);
						});
				}(std::forward<decltype(args)>(args)...);
			}
		};
	}

private:
	template <std::size_t N, std::size_t CurrN, typename... Args>
	struct is_nth_arg_handle_impl;

	template <std::size_t N, std::size_t CurrN, typename Arg, typename... Args>
	struct is_nth_arg_handle_impl<N, CurrN, Arg, Args...>
		: std::conditional_t<
			  N == CurrN,
			  std::is_same<Arg, Handle>,
			  is_nth_arg_handle_impl<N, CurrN + 1, Args...>>
	{
	};

	template <std::size_t N, std::size_t CurrN, typename Arg>
	struct is_nth_arg_handle_impl<N, CurrN, Arg>
		: std::conditional_t<N == CurrN, std::is_same<Arg, Handle>, std::false_type>
	{
	};

	template <std::size_t N, typename... Args>
	struct is_nth_arg_handle : is_nth_arg_handle_impl<N, 0, Args...>
	{
	};

	template <std::size_t N, typename... Args>
	static constexpr auto is_nth_arg_handle_v = is_nth_arg_handle<N, Args...>::value;

	template <typename... Args>
	struct is_0th_arg_error : std::false_type
	{
	};

	template <typename Arg, typename... Args>
	struct is_0th_arg_error<Arg, Args...> : std::is_same<Arg, fp_ErrorMessage *>
	{
	};

	template <typename... Args>
	static constexpr auto is_0th_arg_error_v = is_0th_arg_error<Args...>::value;

	enum class out_param_sig
	{
		cannot_return_cannot_error,
		cannot_return_can_error,
		can_return_cannot_error,
		can_return_can_error,
		unrecognised
	};

	template <typename... Args>
	static constexpr out_param_sig suite_func_sig_type()
	{
		if constexpr (is_nth_arg_handle_v<0, Args...>)
		{
			// fn(handle, args...)
			return out_param_sig::cannot_return_cannot_error;
		}
		else if constexpr (is_0th_arg_error_v<Args...> && is_nth_arg_handle_v<1, Args...>)
		{
			// fn(err, handle, args...)
			return out_param_sig::cannot_return_can_error;
		}
		else if constexpr (!is_0th_arg_error_v<Args...> && is_nth_arg_handle_v<1, Args...>)
		{
			// fn(out, handle, args...)
			return out_param_sig::can_return_cannot_error;
		}
		else if constexpr (is_0th_arg_error_v<Args...> && is_nth_arg_handle_v<2, Args...>)
		{
			// fn(err, out, handle, args...)
			return out_param_sig::can_return_can_error;
		}
		return out_param_sig::unrecognised;
	}
};

}  // namespace feltplugin::service