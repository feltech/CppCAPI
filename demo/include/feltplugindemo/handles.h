#ifndef feltplugin_demo_handles_h
#define feltplugin_demo_handles_h

#include <cstddef>

#include <feltplugin/handles.h>

#ifdef __cplusplus
extern "C"
{
#endif
	// String

	typedef struct fp_String_t * fp_String_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage err, fp_String_h * out, char const * str);
		void (*release)(fp_String_h handle);
		char const * (*c_str)(fp_String_h handle);	// noexcept
	} fp_String_s;

	fp_String_s fp_String_suite();

	// StringDict

	typedef struct fp_StringDict_t * fp_StringDict_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage, fp_StringDict_h *);

		void (*release)(fp_StringDict_h);

		fp_ErrorCode (*insert)(fp_ErrorMessage, fp_StringDict_h, fp_String_h, fp_String_h);

		fp_ErrorCode (*at)(fp_ErrorMessage, fp_String_h *, fp_StringDict_h, fp_String_h);
	} fp_StringDict_s;

	fp_StringDict_s fp_StringDict_suite();

#ifdef __cplusplus
}
#endif
#endif