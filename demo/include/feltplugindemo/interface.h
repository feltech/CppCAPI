#ifndef feltplugin_demo_handles_h
#define feltplugin_demo_handles_h

#include <cstddef>

#include <feltplugin/errors.h>

#ifdef __cplusplus
extern "C"
{
#endif
	// String

	typedef struct fpdemo_String_t * fpdemo_String_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage err, fpdemo_String_h * out, char const * str);
		void (*release)(fpdemo_String_h handle);
		char const * (*c_str)(fpdemo_String_h handle);	// noexcept
		fp_ErrorCode (*at)(fp_ErrorMessage, char *, fpdemo_String_h, int);
	} fpdemo_String_s;

	fpdemo_String_s fpdemo_String_suite();

	// StringDict

	typedef struct fpdemo_StringDict_t * fpdemo_StringDict_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage, fpdemo_StringDict_h *);

		void (*release)(fpdemo_StringDict_h);

		fp_ErrorCode (*insert)(
			fp_ErrorMessage, fpdemo_StringDict_h, fpdemo_String_h, fpdemo_String_h);

		fp_ErrorCode (*at)(
			fp_ErrorMessage, fpdemo_String_h *, fpdemo_StringDict_h, fpdemo_String_h);
	} fp_StringDict_s;

	fp_StringDict_s fpdemo_StringDict_suite();

	// Plugin

	typedef struct fpdemo_Plugin_t * fpdemo_Plugin_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage, fpdemo_Plugin_h *, fpdemo_StringDict_h);

		void (*release)(fpdemo_Plugin_h);

		fp_ErrorCode (*update_dict)(fp_ErrorMessage, fpdemo_Plugin_h, fpdemo_String_h);

	} fpdemo_Plugin_s;

	// Defined within plugin.
	//	fpdemo_Plugin_s fpdemo_Plugin_suite();

#ifdef __cplusplus
}
#endif
#endif