// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the ErrorMap of ErrorTraits to be used by services/clients converting between exceptions
 * and error codes.
 */
#pragma once

#include <cstring>

#include <stdexcept>
#include <type_traits>
#include <utility>
#include "interface.h"

namespace cppcapi
{

/**
 * Traits mapping an exception class to error code.
 *
 * @tparam TException Exception class.
 * @tparam Tcode Error code.
 */
template <class TException, cppcapi_ErrorCode Tcode>
struct ErrorTraits
{
	using Exception = TException;
	static constexpr cppcapi_ErrorCode code = Tcode;
};

/**
 * Default exception type if error code is unrecognized.
 */
struct UnknownError : std::runtime_error
{
	using runtime_error::runtime_error;
};

/**
 * Utility to extract the type/code of an exception.
 *
 * Forward declaration.
 *
 * @tparam ...
 */
template <class...>
struct ErrorMap;

/**
 * Utility functions.
 */
namespace detail
{

inline void extract_message(cppcapi_ErrorMessage & err, const std::string_view msg) noexcept
{
	err.size = std::min(msg.size(), err.capacity);
	strncpy(err.data, msg.data(), err.size);
	err.data[err.capacity - 1] = '\0';
}

inline void extract_exception_message(
	cppcapi_ErrorMessage & err, std::exception const & ex) noexcept
{
	extract_message(err, ex.what());
}

inline void non_exception_message(cppcapi_ErrorMessage & err) noexcept
{
	extract_message(err, "Unknown non-exception error caught");
}

template <class Traits>
void throw_if_matches(cppcapi_ErrorMessage const & err, cppcapi_ErrorCode const code)
{
	if (code == Traits::code)
	{
		using Exception = typename Traits::Exception;

		static_assert(
			std::is_constructible_v<Exception, std::string> || std::is_constructible_v<Exception>,
			"Exception type must be either default constructible or constructible from a "
			"std::string");

		if constexpr (std::is_constructible_v<Exception, std::string>)
		{
			throw Exception{{err.data, err.size}};
		}
		else if constexpr (std::is_constructible_v<Exception>)
		{
			throw Exception{};
		}
	}
}

template <class Outer, class Inner>
static decltype(auto) decorate(Outer && outer, Inner && inner)
{
	return [&outer, &inner] { return outer(std::forward<Inner>(inner)); };
}

template <class Outer, class... Inners>
static decltype(auto) decorate(Outer && outer, Inners &&... inners)
{
	return [&outer, &inners...] { return outer(decorate(std::forward<Inners>(inners)...)); };
}
}  // namespace detail

/**
 * Utility to extract the type/code of an exception.
 *
 * This is the default error map unless overridden when specialising HandleConverter/SuiteDecorator.
 *
 * Catches `std::exception` (and derived), raises `UnknownError`, and associates with error
 * code cppcapi_error.
 */
template <>
struct ErrorMap<>
{
	/**
	 * Execute a callable, converting any thrown exception to an error code and message.
	 *
	 * This default implementation catches all exceptions and returns cppcapi_error on error.
	 *
	 * @tparam Fn Callable type to execute.
	 * @param err Storage for error message.
	 * @param fn Callback to execute.
	 * @return cppcapi_ok if no exception was raised, cppcapi_error otherwise.
	 */
	template <typename Fn>
	static cppcapi_ErrorCode wrap_exception(cppcapi_ErrorMessage & err, Fn && fn)
	{
		try
		{
			fn();
		}
		catch (std::exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return cppcapi_error;
		}
		return cppcapi_ok;
	}

	/**
	 * Throw exception if given error code matches.
	 *
	 * This default implementation throws UnknownError with given message if code is anything
	 * other than cppcapi_ok.
	 *
	 * @param err Storage for error message.
	 * @param code Error code.
	 */
	static constexpr void throw_exception(
		cppcapi_ErrorMessage const & err, cppcapi_ErrorCode const code)
	{
		if (code != cppcapi_ok)
			throw UnknownError{{err.data, err.size}};
	};
};

/**
 * Utility to extract the type/code of an exception.
 *
 * This is a single-element error map.
 *
 * @tparam Traits ErrorTraits of exception to be matched.
 */
template <class Traits>
struct ErrorMap<Traits>
{
	/**
	 * Execute a callable, converting any thrown exception to an error code and message.
	 *
	 * If thrown exception matches Traits::Exception then returns Traits::code, otherwise returns
	 * cppcapi_error.
	 *
	 * @tparam Fn Callable type to execute.
	 * @param err Storage for error message.
	 * @param fn Callback to execute.
	 * @return cppcapi_ok if no exception was raised, error code otherwise.
	 */
	template <typename Fn>
	static cppcapi_ErrorCode wrap_exception(cppcapi_ErrorMessage & err, Fn && fn)
	{
		try
		{
			fn();
		}
		catch (typename Traits::Exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return Traits::code;
		}
		catch (std::exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return cppcapi_error;
		}
		catch (...)
		{
			detail::non_exception_message(err);
			return cppcapi_error;
		}
		return cppcapi_ok;
	}

	/**
	 * Throw exception if given error code matches.
	 *
	 * If the code does not match, calls through to the unmatched case, ErrorMap<>.
	 *
	 * @param err Storage for error message.
	 * @param code Error code.
	 */
	static constexpr void throw_exception(
		cppcapi_ErrorMessage const & err, cppcapi_ErrorCode const code)
	{
		detail::throw_if_matches<Traits>(err, code);
		ErrorMap<>::throw_exception(err, code);
	};
};

/**
 * Utility to extract the type/code of an exception.
 *
 * This is a multi-element error map, where multiple ErrorTraits are given for matching against
 * thrown exceptions.
 *
 * @tparam Traits List of ErrorTraits
 */
template <class... Traits>
struct ErrorMap
{
	/**
	 * Execute a callable, converting any thrown exception to an error code and message.
	 *
	 * Recursively decorates the callable with an exception handler for each of the provided
	 * ErrorTraits.
	 *
	 * @tparam Fn Callable type to execute.
	 * @param err Storage for error message.
	 * @param fn Callback to execute.
	 * @return cppcapi_ok if no exception was raised, error code otherwise.
	 */
	template <typename Fn>
	static cppcapi_ErrorCode wrap_exception(cppcapi_ErrorMessage & err, Fn && fn)
	{
		try
		{
			return detail::decorate(
				[&err](auto && wrapped)
				{
					try
					{
						if constexpr (std::is_void_v<decltype(wrapped())>)
						{
							// Leaf ErrorTraits case.
							wrapped();
						}
						else
						{
							// Recursive ErrorTraits case.
							return wrapped();
						}
					}
					catch (typename Traits::Exception const & ex)
					{
						detail::extract_exception_message(err, ex);
						return Traits::code;
					}
					return cppcapi_ok;
				}...,
				fn)();
		}
		// Fallthrough catch-all if no ErrorMap matches.
		catch (std::exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return cppcapi_error;
		}
		catch (...)
		{
			detail::non_exception_message(err);
			return cppcapi_error;
		}
	}

	/**
	 * Throw exception if given error code matches.
	 *
	 * Will attempt to match against each of the ErrorTraits in turn, before falling back to the
	 * default ErrorMap<> handler.
	 *
	 * @param err Storage for error message.
	 * @param code Error code.
	 */
	static constexpr void throw_exception(
		cppcapi_ErrorMessage const & err, cppcapi_ErrorCode const code)
	{
		if (code == cppcapi_ok)
			return;

		(detail::throw_if_matches<Traits>(err, code), ..., ErrorMap<>::throw_exception(err, code));
	};
};
}  // namespace cppcapi
