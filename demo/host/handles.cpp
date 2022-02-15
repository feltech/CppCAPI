#include "handles.hpp"
#include "feltplugindemohostlib_export.h"

#include <feltplugindemo/handles.h>
#include <feltplugin/errors.hpp>
#include <feltplugin/owner/handle_factory.hpp>

extern "C"
{
	using feltplugindemohost::HandleFactory;

	// String

	using feltplugindemohost::String;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_String_s fp_String_suite()
	{
		return {
			[](fp_ErrorMessage err, fp_String_h * out, char const * str)
			{ return HandleFactory<fp_String_h>::make(err, out, str); },

			&HandleFactory<fp_String_h>::release,

			[](fp_String_h handle) { return HandleFactory<fp_String_h>::convert(handle)->c_str(); }};
	}

	// StringDict

	using feltplugindemohost::StringDict;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_StringDict_s fp_StringDict_suite()
	{
		return {
			[](fp_ErrorMessage err, fp_StringDict_h * out)
			{ return HandleFactory<fp_StringDict_h>::make(err, out); },

			&HandleFactory<fp_StringDict_h>::release,

			[](fp_ErrorMessage err, fp_StringDict_h handle, fp_String_h key, fp_String_h value)
			{
				using feltplugin::wrap_exception;
				return wrap_exception(
					err,
					[handle, key, value]
					{
						String const & key_str = *HandleFactory<fp_String_h>::convert(key);
						String const & value_str = *HandleFactory<fp_String_h>::convert(value);
						StringDict & dict = *HandleFactory<fp_StringDict_h>::convert(handle);

						dict.insert({key_str, value_str});
					});
			},

			[](fp_ErrorMessage err, fp_String_h * out, fp_StringDict_h handle, fp_String_h key)
			{
				using feltplugin::wrap_exception;
				return wrap_exception(
					err,
					[out, handle, key]
					{
						String const & key_str = *HandleFactory<fp_String_h>::convert(key);
						StringDict const & dict = *HandleFactory<fp_StringDict_h>::convert(handle);

						std::string value = dict.at(key_str);

						*out = HandleFactory<fp_String_h>::make(std::move(value));
					});
			}};
	}
}
