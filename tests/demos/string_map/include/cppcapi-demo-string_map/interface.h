// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#ifndef cppcapi_demo_handles_h
#define cppcapi_demo_handles_h

#include <stddef.h>	 // NOLINT(modernize-deprecated-headers)

#include <cppcapi/interface.h>

#ifdef __cplusplus
extern "C"
{
#endif
	// TODO: Add const to handles where appropriate

	// StringView

	typedef struct cppcapidemo_StringView_t * cppcapidemo_StringView_h;

	typedef struct
	{
		char const * (*data)(cppcapidemo_StringView_h);	 // noexcept
		size_t (*size)(cppcapidemo_StringView_h);		 // noexcept
	} cppcapidemo_StringView_s;

	cppcapidemo_StringView_s cppcapidemo_StringView_suite();

	// String

	typedef struct cppcapidemo_String_t * cppcapidemo_String_h;

	typedef struct
	{
		cppcapidemo_String_h (*create)();
		void (*release)(cppcapidemo_String_h);
		cppcapi_ErrorCode (*assign_cstr)(
			cppcapi_ErrorMessage *, cppcapidemo_String_h, char const *);
		cppcapi_ErrorCode (*assign_StringView)(
			cppcapi_ErrorMessage *, cppcapidemo_String_h, cppcapidemo_StringView_h);
		char const * (*c_str)(cppcapidemo_String_h);  // noexcept
		cppcapi_ErrorCode (*at)(cppcapi_ErrorMessage *, char *, cppcapidemo_String_h, size_t);
	} cppcapidemo_String_s;

	cppcapidemo_String_s cppcapidemo_String_suite();

	// StringDict

	typedef struct cppcapidemo_StringDict_t * cppcapidemo_StringDict_h;

	typedef struct
	{
		cppcapi_ErrorCode (*create)(cppcapi_ErrorMessage *, cppcapidemo_StringDict_h *);

		void (*release)(cppcapidemo_StringDict_h);

		cppcapi_ErrorCode (*insert)(
			cppcapi_ErrorMessage *,
			cppcapidemo_StringDict_h,
			cppcapidemo_String_h,
			cppcapidemo_String_h);

		cppcapi_ErrorCode (*at)(
			cppcapi_ErrorMessage *,
			cppcapidemo_String_h *,
			cppcapidemo_StringDict_h,
			cppcapidemo_String_h);
	} cppcapidemo_StringDict_s;

	cppcapidemo_StringDict_s cppcapidemo_StringDict_suite();

	// Worker

	typedef struct cppcapidemo_Worker_t * cppcapidemo_Worker_h;

	typedef struct
	{
		cppcapi_ErrorCode (*create)(
			cppcapi_ErrorMessage *, cppcapidemo_Worker_h *, cppcapidemo_StringDict_h);

		void (*release)(cppcapidemo_Worker_h);

		cppcapi_ErrorCode (*update_dict)(
			cppcapi_ErrorMessage *, cppcapidemo_Worker_h, cppcapidemo_StringView_h);

	} cppcapidemo_Worker_s;

	// Defined within plugin.
	//	cppcapidemo_Worker_s cppcapidemo_Worker_suite();

#ifdef __cplusplus
}
#endif
#endif