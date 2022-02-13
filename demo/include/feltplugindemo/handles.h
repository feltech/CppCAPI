#ifndef feltplugin_demo_handles_h
#define feltplugin_demo_handles_h

#include <cstddef>

#include <feltplugin/errors.h>

#ifdef __cplusplus
extern "C"
{
#endif
	// String

	typedef struct fp_String_t * fp_String;

	fp_ErrorCode fp_String_create(fp_ErrorMessage err, fp_String * out, char const * str);

	void fp_String_release(fp_String handle);

	char const * fp_String_cstr(fp_String handle);	// noexcept

	// StringView

	typedef struct fp_StringView_t * fp_StringView;

	fp_ErrorCode fp_StringView_create(
		fp_ErrorMessage err, fp_StringView * out, char const * str, size_t size);

	void fp_StringView_release(fp_StringView handle);

	char const * fp_StringView_data(fp_StringView handle);	// noexcept

	size_t fp_StringView_size(fp_StringView handle);		// noexcept

	// StringDict

	typedef struct fp_StringDict_t * fp_StringDict;

	fp_ErrorCode fp_StringDict_create(fp_ErrorMessage err, fp_StringDict * out);

	void fp_StringDict_release(fp_StringDict handle);

	fp_ErrorCode fp_StringDict_insert(
		fp_ErrorMessage err, fp_StringDict handle, fp_String key, fp_String value);

	fp_ErrorCode fp_StringDict_at(
		fp_ErrorMessage err, fp_String * out, fp_StringDict handle, fp_String key);
#ifdef __cplusplus
}
#endif
#endif