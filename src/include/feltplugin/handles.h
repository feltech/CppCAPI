#ifndef feltplugin_handles_h
#define feltplugin_handles_h

#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef enum
	{
		fp_ok = 1,
		fp_error = 2
	} fp_ErrorCode;

	typedef char fp_ErrorMessage[500];
#ifdef __cplusplus
}
#endif
#endif