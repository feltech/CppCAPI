// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains some common C types and values to be used by C interfaces.
 */
#ifndef cppcapi_handles_h
#define cppcapi_handles_h

#include <stddef.h>	 // NOLINT(modernize-deprecated-headers)

#ifdef __cplusplus
extern "C"
{
#endif
	/// Type to use when returning an error code.
	typedef int cppcapi_ErrorCode;

	/**
	 * Storage for error messages.
	 *
	 * @warning This type should not be used for arguments other than the first argument of a suite
	 * function, or template matches might fail.
	 */
	typedef struct
	{
		size_t capacity;
		size_t size;
		char * data;
	} cppcapi_ErrorMessage;

	/// Error code signaling no error occurred.
	static const cppcapi_ErrorCode cppcapi_ok = 0;
	/// Default error code signalling some error occurred. Expected to be extended by ErrorMap.
	static const cppcapi_ErrorCode cppcapi_error = 1;
#ifdef __cplusplus
}
#endif
#endif