#ifndef feltplugin_handles_h
#define feltplugin_handles_h

#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef int fp_ErrorCode;
	typedef char fp_ErrorMessage[500];

	static const fp_ErrorCode fp_ok = 0;
	static const fp_ErrorCode fp_error = 1;
#ifdef __cplusplus
}
#endif
#endif