#include "handles.hpp"
#include "feltplugindemohostlib_export.h"

#include <feltplugindemo/handles.h>
#include <feltplugin/exceptions.hpp>
#include <feltplugin/host/handle_factory.hpp>

extern "C"
{
	using feltplugindemohost::HandleFactory;

	// String

	using feltplugindemohost::String;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_ErrorCode
	fp_String_create(fp_ErrorMessage err, fp_String * out, char const * str)
	{
		return HandleFactory<String>::make(err, out, str);
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT void fp_String_release(fp_String handle)
	{
		using feltplugindemohost::String;

		HandleFactory<String>::release(handle);
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT char const * fp_String_cstr(fp_String handle)
	{
		using feltplugindemohost::String;

		return HandleFactory<String>::convert(handle)->c_str();
	}

	// StringView

	using feltplugindemohost::StringView;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_ErrorCode
	fp_StringView_create(char * err, fp_StringView * out, char const * str, size_t size)
	{
		return HandleFactory<StringView>::make(err, out, str, size);
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT void fp_StringView_release(fp_StringView handle)
	{
		HandleFactory<StringView>::release(handle);
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT char const * fp_StringView_data(fp_StringView handle)
	{
		return HandleFactory<StringView>::convert(handle)->data();
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT size_t fp_StringView_size(fp_StringView handle)
	{
		return HandleFactory<StringView>::convert(handle)->size();
	}

	// StringDict

	using feltplugindemohost::StringDict;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_ErrorCode
	fp_StringDict_create(fp_ErrorMessage err, fp_StringDict * out)
	{
		return HandleFactory<StringDict>::make(err, out);
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT void fp_StringDict_release(fp_StringDict handle)
	{
		HandleFactory<StringDict>::release(handle);
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_ErrorCode
	fp_StringDict_insert(fp_ErrorMessage err, fp_StringDict handle, fp_String key, fp_String value)
	{
		using feltplugin::wrap_exception;
		return wrap_exception(
			err,
			[handle, key, value]
			{
				String const & key_str = *HandleFactory<String>::convert(key);
				String const & value_str = *HandleFactory<String>::convert(value);
				StringDict & dict = *HandleFactory<StringDict>::convert(handle);

				dict.insert({key_str, value_str});
			});
	}

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_ErrorCode
	fp_StringDict_at(fp_ErrorMessage err, fp_String * out, fp_StringDict handle, fp_String key)
	{
		using feltplugin::wrap_exception;
		return wrap_exception(
			err,
			[out, handle, key]
			{
				String const & key_str = *HandleFactory<String>::convert(key);
				StringDict const & dict = *HandleFactory<StringDict>::convert(handle);

				std::string value = dict.at(key_str);

				*out = HandleFactory<String>::make(std::move(value));
			});
	}
}
