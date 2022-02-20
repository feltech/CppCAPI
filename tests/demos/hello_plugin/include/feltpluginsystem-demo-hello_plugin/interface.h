#ifndef feltplugin_demo_handles_h
#define feltplugin_demo_handles_h

#include <feltpluginsystem/interface.h>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct fpdemo_Worker_t * fpdemo_Worker_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage, fpdemo_Worker_h *);
		void (*release)(fpdemo_Worker_h);
		void (*work)(fpdemo_Worker_h);	 // noexcept
	} fpdemo_Worker_s;

	// Host will expect plugin to define:
	// fpdemo_Worker_s fpdemo_Worker_suite();

#ifdef __cplusplus
}
#endif
#endif