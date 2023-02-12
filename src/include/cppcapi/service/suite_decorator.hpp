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

	template <auto fn>
	struct free_fn_ptr_t : std::integral_constant<decltype(fn), fn>
	{
		static_assert(
			std::is_pointer_v<decltype(fn)> &&
				std::is_function_v<std::remove_pointer_t<decltype(fn)>>,
			"free_fn_ptr must only be used with free function pointers");
	};

public:
	template <auto fn>
	static constexpr mem_fn_ptr_t<fn> mem_fn_ptr{};

	template <auto fn>
	static constexpr free_fn_ptr_t<fn> free_fn_ptr{};

	/**
	 * Create a new instance, where the constructor can throw, storing associated handle in
	 * out-parameter.
	 */
	template <typename... Args>
	static cppcapi_ErrorCode create(cppcapi_ErrorMessage * err, Handle * out, Args... args)
	{
		return HandleManager<Handle>::create(err, out, std::forward<Args>(args)...);
	}

	/// Create a new instance, storing associated handle in out-parameter.
	template <typename... Args>
	static void create(Handle * out, Args... args)
	{
		HandleManager<Handle>::create(out, std::forward<Args>(args)...);
	}

	/// Create a new instance, returning associated handle.
	template <typename... Args>
	static Handle create(Args... args)
	{
		Handle out;
		HandleManager<Handle>::create(&out, std::forward<Args>(args)...);
		return out;
	}

	/// Release the handle to an instance, potentially destroying the instance.
	static void release(Handle handle)
	{
		HandleManager<Handle>::release(handle);
	}

	/**
	 * Adapt a suite function to have a more C++-like interface, automatically converting
	 * handles.
	 *
	 * The arguments will be converted from handles to objects and the return value converted from
	 * an object to a handle, as appropriate.
	 *
	 * This specialisation decorates a free function provided as a template argument to this
	 * `SuiteDecorator`.
	 *
	 * The function to decorate must be wrapped in a `free_fn_ptr` before being passed to
	 * this function, e.g. `decorate(free_fn_ptr<&my_func>)`, in order for compile-time
	 * template deduction to work.
	 *
	 * @tparam ReturnHandle Type of handle of return value, void (default) for non-handle return
	 * type.
	 * @tparam fn Member function pointer, deduced from `mem_fn_ptr_const` function parameter.
	 * @param free_fn_ptr_const Not used directly, used instead to deduce the `fn` template
	 * parameter.
	 * @return Non-capturing lambda satisfying C function signature.
	 */
	template <typename ReturnHandle = void, auto fn = nullptr>
	static auto decorate([[maybe_unused]] free_fn_ptr_t<fn> free_fn_ptr_const)
	{
		return decorate<fn, ReturnHandle>();
	}

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
	 * @tparam ReturnHandle Type of handle of return value, void (default) for non-handle return
	 * type.
	 * @tparam Callable Stateless callable type to decorate.
	 * @param lambda Stateless callable to decorate.
	 * @return Non-capturing lambda satisfying C function signature.
	 */
	template <typename ReturnHandle = void, typename Callable = void>
	static auto decorate([[maybe_unused]] Callable && lambda)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();

		static_assert(
			std::is_empty_v<Callable>,
			"Only stateless callable objects (i.e. non-capturing lambdas) can be passed directly");

		return decorate<
			lambda_wrapper_t<Callable, decltype(std::function{lambda})>::call,
			ReturnHandle>();
	}

	/**
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
	 * @tparam ReturnHandle Type of handle of return value, void (default) for non-handle return
	 * type.
	 * @tparam fn Member function pointer, deduced from `mem_fn_ptr_const` function parameter.
	 * @param mem_fn_ptr_const Not used directly, used instead to deduce the `fn` template
	 * parameter.
	 * @return Non-capturing lambda satisfying C function signature.
	 */
	template <typename ReturnHandle = void, auto fn = nullptr>
	static auto decorate([[maybe_unused]] mem_fn_ptr_t<fn> mem_fn_ptr_const)
	{
		return decorate<fn, ReturnHandle>();
	}

	template <auto fn, typename ReturnHandle = void>
	static auto decorate()
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();
		static_assert(
			std::is_member_function_pointer_v<decltype(fn)> ||
				(std::is_pointer_v<decltype(fn)> &&
				 std::is_function_v<std::remove_pointer_t<decltype(fn)>>),
			"Can only decorate function pointers");

		return [](auto... args)
		{
			static constexpr out_param_sig sig_type = suite_func_sig_type<decltype(args)...>();
			static_assert(sig_type != out_param_sig::unrecognised, "Ill-formed C suite function");

			if constexpr (sig_type == out_param_sig::cannot_output_cannot_error)
			{
				return [](Handle handle, auto &&... rest)
				{
					// Although `cannot_output_cannot_error`, this refers to out-parameter, the
					// function may still return a value.
					return convert_and_call<ReturnHandle>(
						fn, handle, std::forward<decltype(rest)>(rest)...);
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::cannot_output_can_error)
			{
				return [](cppcapi_ErrorMessage * err, Handle handle, auto &&... rest)
				{
					return TErrorMap::wrap_exception(
						*err,
						[&]
						{ convert_and_call(fn, handle, std::forward<decltype(rest)>(rest)...); });
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_output_cannot_error)
			{
				return [](auto * out, Handle handle, auto &&... rest)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					*out = convert_and_call<Out>(fn, handle, std::forward<decltype(rest)>(rest)...);
				}(std::forward<decltype(args)>(args)...);
			}
			else if constexpr (sig_type == out_param_sig::can_output_can_error)
			{
				return [](cppcapi_ErrorMessage * err, auto * out, Handle handle, auto &&... rest)
				{
					using Out = std::remove_pointer_t<decltype(out)>;

					return TErrorMap::wrap_exception(
						*err,
						[&] {
							*out = convert_and_call<Out>(
								fn, handle, std::forward<decltype(rest)>(rest)...);
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

	/// Call a C++ function after converting C handles to their C++ types.
	template <typename ReturnHandle = void, typename Fn = void, typename... CArg>
	static decltype(auto) convert_and_call(Fn && fn, CArg &&... arg)
	{
		auto const call = [&]() -> decltype(auto)
		{
			if constexpr (std::is_member_function_pointer_v<Fn>)
			{
				return convert_and_call_helper_t<Fn>::call(
					std::forward<Fn>(fn), std::forward<CArg>(arg)...);
			}
			else
			{
				return convert_and_call_helper_t<decltype(std::function{fn})>::call(
					std::forward<Fn>(fn), std::forward<CArg>(arg)...);
			}
		};

		if constexpr (std::is_void_v<ReturnHandle>)
		{
			return call();
		}
		else
		{
			// Return handle type specified in optional template param, so convert.

			using ReturnType = decltype(call());

			if constexpr (HandleManager<ReturnHandle>::is_owned_by_client())
			{
				return HandleManager<ReturnHandle>::make_to_handle(call());
			}
			else if constexpr (HandleManager<ReturnHandle>::is_owned_by_service())
			{
				static_assert(
					std::is_reference_v<ReturnType>,
					"Attempting to return a handle to a temporary");

				return HandleManager<ReturnHandle>::to_handle(call());
			}
			else if constexpr (HandleManager<ReturnHandle>::is_shared_ownership())
			{
				if constexpr (HandleManager<ReturnHandle>::template is_shared_ptr<ReturnType>())
				{
					return HandleManager<ReturnHandle>::to_handle(call());
				}
				else
				{
					return HandleManager<ReturnHandle>::make_to_handle(call());
				}
			}
			else
			{
				// ReturnHandle given but for unrecognized type, so assume C-native return type.
				return call();
			}
		}
	}

	template <typename>
	struct convert_and_call_helper_t;

	/// Helper for non-member functions (abuses std::function to get args).
	template <typename Ret, typename... CppArg>
	struct convert_and_call_helper_t<std::function<Ret(CppArg...)>>
	{
		template <typename Fn, typename... CArg>
		static decltype(auto) call(Fn && fn, CArg &&... arg)
		{
			return fn(HandleManager<std::decay_t<CArg>>::template to_instance_or_ptr<CppArg>(
				std::forward<CArg>(arg))...);
		}
	};

	/// Helper for member function.
	template <typename Ret, typename Class, typename... CppArg>
	struct convert_and_call_helper_t<Ret (Class::*)(CppArg...)>
	{
		template <typename Fn, typename Handle, typename... CArg>
		static decltype(auto) call(Fn && fn, Handle handle, CArg &&... arg)
		{
			return std::mem_fn(fn)(
				HandleManager<Handle>::template to_instance(std::forward<Handle>(handle)),
				HandleManager<std::decay_t<CArg>>::template to_instance_or_ptr<CppArg>(
					std::forward<CArg>(arg))...);
		}
	};

	/// Helper for const member function.
	template <typename Ret, typename Class, typename... CppArg>
	struct convert_and_call_helper_t<Ret (Class::*)(CppArg...) const>
		: convert_and_call_helper_t<Ret (Class::*)(CppArg...)>
	{
	};

	/// Helper for noexcept member function.
	template <typename Ret, typename Class, typename... CppArg>
	struct convert_and_call_helper_t<Ret (Class::*)(CppArg...) noexcept>
		: convert_and_call_helper_t<Ret (Class::*)(CppArg...)>
	{
	};

	/// Helper for const noexcept member function.
	template <typename Ret, typename Class, typename... CppArg>
	struct convert_and_call_helper_t<Ret (Class::*)(CppArg...) const noexcept>
		: convert_and_call_helper_t<Ret (Class::*)(CppArg...)>
	{
	};

	template <class Lambda, class Sig>
	struct lambda_wrapper_t;

	/**
	 * Compile-time lambda helper.
	 *
	 * This class's `call` static member function is suitable for use in an `auto` template
	 * parameter (i.e. as a function pointer). This works around the limitation that the
	 * address of a lambda object is not a compile-time constant.
	 *
	 * To allow deduction down the line to work, we must know the args of the lambda ahead of time,
	 * hence we abuse std::function's compile-time deduction guides to inform us.
	 *
	 * The lambda type _must_ be non-capturing for this to work, so it is safe to cast to.
	 */
	template <class Lambda, typename Ret, typename... Args>
	struct lambda_wrapper_t<Lambda, std::function<Ret(Args...)>>
	{
		using Self = lambda_wrapper_t<Lambda, std::function<Ret(Args...)>>;
		static_assert(std::is_empty_v<Lambda>, "Cannot wrap a stateful (i.e. capturing) lambda");

		static Ret call(Args... args)
		{
			const Self self;
			return reinterpret_cast<const Lambda &>(self)(std::forward<Args>(args)...);
		}
	};
};

}  // namespace cppcapi::service
