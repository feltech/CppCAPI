// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#ifndef cppcapi_demo_handles_h
#define cppcapi_demo_handles_h

#include <cppcapi/interface.h>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct cppcapidemo_Worker_t * cppcapidemo_Worker_h;

	typedef struct
	{
		cppcapi_ErrorCode (*create)(cppcapi_ErrorMessage *, cppcapidemo_Worker_h *);
		void (*release)(cppcapidemo_Worker_h);
		void (*work)(cppcapidemo_Worker_h);	 // noexcept
	} cppcapidemo_Worker_s;

	// Host will expect plugin to define:
	// cppcapidemo_Worker_s cppcapidemo_Worker_suite();

#ifdef __cplusplus
}
#endif
#endif