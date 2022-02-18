#ifndef feltplugin_demo_handles_h
#define feltplugin_demo_handles_h

#include <cstddef>

#include <feltplugin/errors.h>

#ifdef __cplusplus
extern "C"
{
#endif
	// TODO: Add const to handles where appropriate

	// StringView

	typedef struct fpdemo_StringView_t * fpdemo_StringView_h;

	typedef struct
	{
		char const * (*data)(fpdemo_StringView_h);	// noexcept
		size_t (*size)(fpdemo_StringView_h);	// noexcept
	} fpdemo_StringView_s;

	fpdemo_StringView_s fpdemo_StringView_suite();

	// String

	typedef struct fpdemo_String_t * fpdemo_String_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage, fpdemo_String_h *);
		void (*release)(fpdemo_String_h);
		fp_ErrorCode (*assign_cstr)(fp_ErrorMessage, fpdemo_String_h, char const *);
		fp_ErrorCode (*assign_StringView)(fp_ErrorMessage, fpdemo_String_h, fpdemo_StringView_h);
		char const * (*c_str)(fpdemo_String_h);	 // noexcept
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

	// Worker

	typedef struct fpdemo_Worker_t * fpdemo_Worker_h;

	typedef struct
	{
		fp_ErrorCode (*create)(fp_ErrorMessage, fpdemo_Worker_h *, fpdemo_StringDict_h);

		void (*release)(fpdemo_Worker_h);

		fp_ErrorCode (*update_dict)(fp_ErrorMessage, fpdemo_Worker_h, fpdemo_StringView_h);

	} fpdemo_Worker_s;

	// Defined within plugin.
	//	fpdemo_Worker_s fpdemo_Worker_suite();

#ifdef __cplusplus
}
#endif
#endif