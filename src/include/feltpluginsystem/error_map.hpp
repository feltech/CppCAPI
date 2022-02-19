#pragma once

#include <cstring>

#include <stdexcept>
#include <type_traits>
#include <utility>
#include "interface.h"

namespace feltplugin
{

template <class TException, fp_ErrorCode Tcode>
struct ErrorTraits
{
	using Exception = TException;
	static constexpr fp_ErrorCode code = Tcode;
};

template <class Traits, class... Rest>
struct ErrorMap
{
	using Exception = typename Traits::Exception;
	static constexpr fp_ErrorCode kCode = Traits::code;

	template <typename Fn>
	static fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
	{
		try
		{
			return ErrorMap<Rest...>::wrap_exception(err, [&fn] { fn(); });
		}
		catch (Exception const & ex)
		{
			return code_from_exception(ex, err);
		}
	}

	template <class ErrorToLookup>
	static fp_ErrorCode code_from_exception(ErrorToLookup const & ex, fp_ErrorMessage err)
	{
		if constexpr (!std::is_same_v<ErrorToLookup, Exception>)
			return ErrorMap<Rest...>::code_from_exception(ex);

		strncpy(err, ex.what(), sizeof(fp_ErrorMessage) - 1);
		err[sizeof(fp_ErrorMessage) - 1] = '\0';

		return kCode;
	}
	static constexpr void exception_from_code(fp_ErrorCode const code, fp_ErrorMessage const & err)
	{
		if (code != kCode)
			ErrorMap<Rest...>::exception_from_code(code, err);
		throw Exception{err};
	};
};

template <class Traits>
struct ErrorMap<Traits>
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
			return code_from_exception(ex, err);
		}
		return fp_ok;
	}

	template <class ErrorToLookup>
	static fp_ErrorCode code_from_exception(ErrorToLookup const & ex, fp_ErrorMessage err)
	{
		strncpy(err, ex.what(), sizeof(fp_ErrorMessage) - 1);
		err[sizeof(fp_ErrorMessage) - 1] = '\0';

		if constexpr (!std::is_same_v<ErrorToLookup, Exception>)
			return fp_error;

		return kCode;
	}

	static constexpr void exception_from_code(fp_ErrorCode const code, fp_ErrorMessage const & err)
	{
		if (code == kCode)
			throw Exception{err};

		throw std::runtime_error{err};
	};
};

using DefaultErrorMap = ErrorMap<
	// Catch-all exception.
	ErrorTraits<std::runtime_error, fp_error>>;
}  // namespace feltplugin