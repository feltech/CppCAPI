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

namespace feltplugin
{

/**
 * Traits mapping an exception class to error code.
 *
 * @tparam TException Exception class.
 * @tparam Tcode Error code.
 */
template <class TException, fp_ErrorCode Tcode>
struct ErrorTraits
{
	using Exception = TException;
	static constexpr fp_ErrorCode code = Tcode;
};

namespace detail
{
inline void extract_exception_message(fp_ErrorMessage err, std::exception const & ex) noexcept
{
	strncpy(err, ex.what(), sizeof(fp_ErrorMessage) - 1);
	err[sizeof(fp_ErrorMessage) - 1] = '\0';
}
}  // namespace detail

/**
 * Utility to extract the type/code of an exception.
 *
 * Default implementation, will never be instantiated.
 *
 * @tparam ...
 */
template <class...>
struct ErrorMap;

/**
 * Utility to extract the type/code of an exception.
 *
 * This is the default error map unless overridden in HandleFactory.
 *
 * Catches `std::exception` (and derived), raises `std::runtime_error`, and associates with error
 * code fp_error.
 */
template <>
struct ErrorMap<>
{
	/**
	 * Execute a callable, converting any thrown exception to an error code and message.
	 *
	 * This default implementation catches all exceptions and returns fp_error on error.
	 *
	 * @tparam Fn Callable type to execute.
	 * @param err Storage for error message.
	 * @param fn Callback to execute.
	 * @return fp_ok if no exception was raised, fp_error otherwise.
	 */
	template <typename Fn>
	static fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
	{
		try
		{
			fn();
		}
		catch (std::exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return fp_error;
		}
		return fp_ok;
	}

	/**
	 * Throw exception if given error code matches.
	 *
	 * This default implementation throws std::runtime_error with given message if code is anything
	 * other than fp_ok.
	 *
	 * @param err Storage for error message.
	 * @param code Error code.
	 */
	static constexpr void throw_exception(fp_ErrorMessage err, fp_ErrorCode const code)
	{
		if (code != fp_ok)
			throw std::runtime_error{err};
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
	/// Hoist exception type from traits.
	using Exception = typename Traits::Exception;
	/// Hoist exception code from traits.
	static constexpr fp_ErrorCode kCode = Traits::code;

	/**
	 * Execute a callable, converting any thrown exception to an error code and message.
	 *
	 * If thrown exception matches Exception then returns kCode, otherwise returns fp_error.
	 *
	 * @tparam Fn Callable type to execute.
	 * @param err Storage for error message.
	 * @param fn Callback to execute.
	 * @return fp_ok if no exception was raised, error code otherwise.
	 */
	template <typename Fn>
	static fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
	{
		try
		{
			fn();
		}
		catch (Exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return kCode;
		}
		catch (std::exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return fp_error;
		}
		return fp_ok;
	}

	/**
	 * Throw exception if given error code matches.
	 *
	 * If the code does not match, calls through to the unmatched case, ErrorMap<>.
	 *
	 * @param err Storage for error message.
	 * @param code Error code.
	 */
	static constexpr void throw_exception(fp_ErrorMessage err, fp_ErrorCode const code)
	{
		if (code == kCode)
			throw Exception{err};

		ErrorMap<>::throw_exception(err, code);
	};
};

namespace detail
{
template <class Traits, class... Rest>
struct ErrorMapRecursive
{
	using Exception = typename Traits::Exception;
	static constexpr fp_ErrorCode kCode = Traits::code;

	template <typename Fn>
	static fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
	{
		try
		{
			return ErrorMapRecursive<Rest...>::wrap_exception(err, [&fn] { fn(); });
		}
		catch (Exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return kCode;
		}
	}
};

template <class Traits>
struct ErrorMapRecursive<Traits>
{
	using Exception = typename Traits::Exception;
	static constexpr fp_ErrorCode kCode = Traits::code;

	template <typename Fn>
	static fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
	{
		try
		{
			fn();
		}
		catch (Exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return kCode;
		}
		return fp_ok;
	}
};
}  // namespace detail

/**
 * Utility to extract the type/code of an exception.
 *
 * @tparam Traits First ErrorTraits in the list.
 * @tparam Rest Remaining ErrorTraits.
 */
template <class Traits, class... Rest>
struct ErrorMap<Traits, Rest...>
{
	using Exception = typename Traits::Exception;
	static constexpr fp_ErrorCode kCode = Traits::code;

	/**
	 * Execute a callable, converting any thrown exception to an error code and message.
	 *
	 * If thrown exception matches Exception then returns kCode, otherwise recursively calls
	 * with the Rest of the ErrorTraits. If none of them matches, then returns fp_error.
	 *
	 * @tparam Fn Callable type to execute.
	 * @param err Storage for error message.
	 * @param fn Callback to execute.
	 * @return fp_ok if no exception was raised, error code otherwise.
	 */
	template <typename Fn>
	static fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
	{
		try
		{
			return detail::ErrorMapRecursive<Rest...>::wrap_exception(err, [&fn] { fn(); });
		}
		catch (Exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return kCode;
		}
		catch (std::exception const & ex)
		{
			detail::extract_exception_message(err, ex);
			return fp_error;
		}
	}

	/**
	 * Throw exception if given error code matches.
	 *
	 * If the code does not match, recurse through Rest of the ErrorTraits.
	 *
	 * @param err Storage for error message.
	 * @param code Error code.
	 */
	static constexpr void throw_exception(fp_ErrorMessage err, fp_ErrorCode const code)
	{
		if (code == kCode)
			throw Exception{err};

		ErrorMap<Rest...>::throw_exception(err, code);
	};
};
}  // namespace feltplugin