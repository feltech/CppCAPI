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
#include "handle_map.hpp"
#include "handle_manager.hpp"

namespace feltplugin::service
{
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
	class TErrorMap>
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