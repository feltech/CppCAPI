// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains some common C types and values to be used by C interfaces.
 */
#ifndef feltplugin_handles_h
#define feltplugin_handles_h

#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif
	/// Type to use when returning an error code.
	typedef int fp_ErrorCode;
	/// Fixed-size char array to use as storage for error messages.
	typedef char fp_ErrorMessage[500];

	/// Error code signaling no error occurred.
	static const fp_ErrorCode fp_ok = 0;
	/// Default error code signalling some error occurred. Expected to be extended by ErrorMap.
	static const fp_ErrorCode fp_error = 1;
#ifdef __cplusplus
}
#endif
#endif