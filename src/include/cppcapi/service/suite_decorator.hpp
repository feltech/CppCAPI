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
#include "handle_manager.hpp"
#include "handle_map.hpp"

namespace cppcapi::service
{
/**
 * Utility class for decorating callables to be used in function pointer suites.
 *
 * @tparam THandle Opaque handle type.
 * @tparam TServiceHandleMap service::HandleMap for mapping handles to native classes.
 * @tparam TClientHandleMap client::HandleMap for mapping handles to client adapter classes.
 * @tparam TErrorMap ErrorMap for mapping exceptions to error codes.
 */
template <class THandle, class TServiceHandleMap, class TClientHandleMap, class TErrorMap>
struct SuiteDecorator
{
private:
	template <class Handle>
	using HandleManager = HandleManager<Handle, TServiceHandleMap, TClientHandleMap, TErrorMap>;

	using Handle = THandle;
	using Class = typename TServiceHandleMap::template class_from_handle<Handle>;
	using Adapter = typename TClientHandleMap::template class_from_handle<Handle>;

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

	template <typename Fn, typename... Args>
	static constexpr void assert_is_invokable(Fn fn, Handle handle, Args... args)
	{
		using is_callable_invokable_with_converted_args = std::is_invocable<
			decltype(std::forward<Fn>(fn)),
			decltype(HandleManager<Handle>::to_instance(std::forward<Handle>(handle))),
			decltype(HandleManager<decltype(args)>::to_instance(
				std::forward<decltype(args)>(args)))...>;

		static_assert(
			is_callable_invokable_with_converted_args::value,
			"Callable cannot be invoked with resolved suite function args");
	}

public:
	template <auto fn>
	static constexpr mem_fn_ptr_t<fn> mem_fn_ptr{};

	/**
	 * Adapt a suite function to have a more C++-like interface, automatically converting
	 * handles.
	 *
	 * The arguments will be converted from handles to objects and the return value converted from
	 * an object to a handle, as appropriate.
	 *
	 * This specialisation decorates a non-capturing lambda function (or other stateless callable),
	 * converting its arguments and return value. The first parameter of the lambda must be (a
	 * reference to) an instance of the (C++) type associated with the handle type that is
	 * provided as a template argument to this `SuiteDecorator`.
	 *
	 * @tparam Lambda Stateless callable type to decorate.
	 * @param lambda Stateless callable to decorate.
	 * @return Non-capturing lambda satisfying C function signature.
	 */
	template <typename Lambda>
	static auto decorate(Lambda && lambda)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();
		static_assert(
			std::is_empty_v<Lambda>,
			"Stateful callables (e.g. capturing lambdas) are not supported");

		// Save off the lambda as a static. Will only happen once per lambda, since lambda type is
		// unique. Ideally we could use the fact that (non-capturing) lambdas are already global,
		// but the compiler isn't clever enough to notice that here.
		// TODO(DF): Obviously gnu::used is GCC-specific, so check Clang and VS don't have the same
		//  linkage issue requiring a similar workaround. This workaround is requied for GCC 9.4.
		static const Lambda fn [[gnu::used]] = std::forward<Lambda>(lambda);

		return [](auto... args)
		{
			static constexpr out_param_sig sig_type = suite_func_sig_type<decltype(args)...>();
			static_assert(sig_type != out_param_sig::unrecognised, "Ill-formed C suite function");

			if constexpr (sig_type == out_param_sig::cannot_output_cannot_error)
			{
				return [](Handle handle, auto... rest) -> auto
				{
					assert_is_invokable(
						fn, std::forward<Handle>(handle), std::forward<decltype(rest)>(rest)...);

					// The `cannot_return_cannot_error` suite type refers to out-parameters. A suite
					// function that cannot error is free to use its return value for something
					// other than an error code.
					// Note that class types will not be auto-converted to return values (see
					// can_return_* signatures for that), since we cannot know for certain the
					// expected return handle type (the same C++ return type could be associated
					// with multiple C handle types).
					// TODO(DF): is there a way around this? One solution would be a (optional)
					// 	return type template param.
					return fn(
						HandleManager<Handle>::to_instance(handle),
						HandleManager<decltype(rest)>::to_instance(
							std::forward<decltype(rest)>(rest))...);
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::cannot_output_can_error)
			{
				return
					[](cppcapi_ErrorMessage * err, Handle handle, auto... rest) -> cppcapi_ErrorCode
				{
					assert_is_invokable(
						fn, std::forward<Handle>(handle), std::forward<decltype(rest)>(rest)...);

					const auto do_call = [&]
					{
						return fn(
							HandleManager<Handle>::to_instance(handle),
							HandleManager<decltype(rest)>::to_instance(
								std::forward<decltype(rest)>(rest))...);
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
			else if constexpr (sig_type == out_param_sig::can_output_cannot_error)
			{
				return [](auto * out, Handle handle, auto... rest) -> void
				{
					assert_is_invokable(
						fn, std::forward<Handle>(handle), std::forward<decltype(rest)>(rest)...);
					using Out = std::remove_pointer_t<decltype(out)>;

					auto ret =
						fn(HandleManager<Handle>::to_instance(handle),
						   HandleManager<decltype(rest)>::to_instance(
							   std::forward<decltype(rest)>(rest))...);

					*out = HandleManager<Out>::make_to_handle(std::move(ret));
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_output_can_error)
			{
				return [](cppcapi_ErrorMessage * err, auto * out, Handle handle, auto... rest)
						   -> cppcapi_ErrorCode
				{
					assert_is_invokable(
						fn, std::forward<Handle>(handle), std::forward<decltype(rest)>(rest)...);

					using Out = std::remove_pointer_t<decltype(out)>;

					return TErrorMap::wrap_exception(
						*err,
						[handle, &out, &rest...]
						{
							auto ret =
								fn(HandleManager<Handle>::to_instance(handle),
								   HandleManager<decltype(rest)>::to_instance(
									   std::forward<decltype(rest)>(rest))...);

							*out = HandleManager<Out>::make_to_handle(std::move(ret));
						});
				}(std::forward<decltype(args)>(args)...);
			}
		};
	}

	/**
	 *
	 * Adapt a suite function to have a more C++-like interface, automatically converting
	 * handles.
	 *
	 * The arguments will be converted from handles to objects and the return value converted from
	 * an object to a handle, as appropriate.
	 *
	 * This specialisation decorates a member function of the class associated with the handle type
	 * provided as a template argument to this `SuiteDecorator`.
	 *
	 * The member function to decorate must be wrapped in a `mem_fn_ptr_t` before being passed to
	 * this function, e.g. `decorate(mem_fn_ptr_t<&MyClass::my_method>)`, in order for compile-time
	 * template deduction to work.
	 *
	 * @tparam fn Member function pointer, deduced from `mem_fn_ptr_const` function parameter.
	 * @param mem_fn_ptr_const Not used directly, used instead to deduce the `fn` template
	 * parameter.
	 * @return Non-capturing lambda satisfying C function signature.
	 */
	template <auto fn>
	static auto decorate([[maybe_unused]] mem_fn_ptr_t<fn> mem_fn_ptr_const)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();

		return [](auto... args)
		{
			static constexpr out_param_sig sig_type = suite_func_sig_type<decltype(args)...>();
			static_assert(sig_type != out_param_sig::unrecognised, "Ill-formed C suite function");

			if constexpr (sig_type == out_param_sig::cannot_output_cannot_error)
			{
				return [](Handle handle, auto... rest)
				{
					const auto do_call = [&]
					{
						return std::mem_fn(fn)(
							HandleManager<Handle>::to_instance(handle),
							HandleManager<decltype(rest)>::to_instance(
								std::forward<decltype(rest)>(rest))...);
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
						return HandleManager<Ret>::make_to_handle(do_call());
					}
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::cannot_output_can_error)
			{
				return [](cppcapi_ErrorMessage * err, Handle handle, auto... rest)
				{
					const auto do_call = [&]
					{
						return std::mem_fn(fn)(
							HandleManager<Handle>::to_instance(handle),
							HandleManager<decltype(rest)>::to_instance(
								std::forward<decltype(rest)>(rest))...);
					};
					return TErrorMap::wrap_exception(
						*err,
						[&do_call]
						{
							using Ret = decltype(do_call());

							// Although `cannot_output_can_error`, this refers to out-parameter,
							// the function may still return a value, so we must handle both cases.
							if constexpr (std::is_void_v<Ret>)
							{
								do_call();
							}
							else
							{
								return HandleManager<Ret>::make_to_handle(do_call());
							}
						});
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_output_cannot_error)
			{
				return [](auto * out, Handle handle, auto... rest)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					auto ret = std::mem_fn(fn)(
						HandleManager<Handle>::to_instance(handle),
						HandleManager<decltype(rest)>::to_instance(
							std::forward<decltype(rest)>(rest))...);

					*out = HandleManager<Out>::make_to_handle(std::move(ret));
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_output_can_error)
			{
				return [](cppcapi_ErrorMessage * err, auto out, Handle handle, auto... rest)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					return TErrorMap::wrap_exception(
						*err,
						[handle, &out, &rest...]
						{
							auto ret = std::mem_fn(fn)(
								HandleManager<Handle>::to_instance(handle),
								HandleManager<decltype(rest)>::to_instance(
									std::forward<decltype(rest)>(rest))...);

							*out = HandleManager<Out>::make_to_handle(std::move(ret));
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
	static constexpr bool is_nth_arg_handle_v = is_nth_arg_handle<N, Args...>::value;

	template <typename... Args>
	struct is_0th_arg_error : std::false_type
	{
	};

	template <typename Arg, typename... Args>
	struct is_0th_arg_error<Arg, Args...> : std::is_same<Arg, cppcapi_ErrorMessage *>
	{
	};

	template <typename... Args>
	static constexpr bool is_0th_arg_error_v = is_0th_arg_error<Args...>::value;

	enum class out_param_sig
	{
		/// fn(handle, args...) -> T
		cannot_output_cannot_error,
		/// fn(err, handle, args...) -> code
		cannot_output_can_error,
		/// fn(out, handle, args...) -> void
		can_output_cannot_error,
		/// fn(err, out, handle, args...) -> code
		can_output_can_error,
		unrecognised
	};

	template <typename... Args>
	static constexpr out_param_sig suite_func_sig_type()
	{
		if constexpr (is_nth_arg_handle_v<0, Args...>)
		{
			// fn(handle, args...) -> T
			return out_param_sig::cannot_output_cannot_error;
		}
		else if constexpr (is_0th_arg_error_v<Args...> && is_nth_arg_handle_v<1, Args...>)
		{
			// fn(err, handle, args...) -> code
			return out_param_sig::cannot_output_can_error;
		}
		else if constexpr (!is_0th_arg_error_v<Args...> && is_nth_arg_handle_v<1, Args...>)
		{
			// fn(out, handle, args...) -> void
			return out_param_sig::can_output_cannot_error;
		}
		else if constexpr (is_0th_arg_error_v<Args...> && is_nth_arg_handle_v<2, Args...>)
		{
			// fn(err, out, handle, args...) -> code
			return out_param_sig::can_output_can_error;
		}
		return out_param_sig::unrecognised;
	}
};

}  // namespace cppcapi::service