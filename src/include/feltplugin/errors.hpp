#pragma once

#include <cstring>
#include <stdexcept>

#include "errors.h"

namespace feltplugin
{
template <typename Fn>
fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
{
	try
	{
		fn();
	}
	catch (std::exception & ex)
	{
		strncpy(err, ex.what(), sizeof(fp_ErrorMessage));
		return fp_error;
	}
	return fp_ok;
}

inline void throw_on_error(fp_ErrorCode code, fp_ErrorMessage err)
{
	switch (code)
	{
		case fp_ErrorCode::fp_error:
			throw std::runtime_error{err};
		case fp_ErrorCode::fp_ok:
			break;
	}
}


template <class...>
struct always_false_t : std::false_type {};

}  // namespace feltplugin