#include "sender.hpp"
#include "feltplugindemohostlib_export.h"

#include <feltplugindemo/interface.h>
#include <feltplugin/errors.hpp>
#include <feltplugin/owner/handle_factory.hpp>

extern "C"
{
	using feltplugindemohost::sender::HandleFactory;

	// String

	using feltplugindemohost::sender::String;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_String_s fpdemo_String_suite()
	{
		return {
			[](fp_ErrorMessage err, fpdemo_String_h * out, char const * str)
			{ return HandleFactory<fpdemo_String_h>::make(err, out, str); },

			&HandleFactory<fpdemo_String_h>::release,

			[](fpdemo_String_h handle)
			{ return HandleFactory<fpdemo_String_h>::convert(handle)->c_str(); }};
	}

	// StringDict

	using feltplugindemohost::sender::StringDict;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_StringDict_s fpdemo_StringDict_suite()
	{
		return {
			[](fp_ErrorMessage err, fpdemo_StringDict_h * out)
			{ return HandleFactory<fpdemo_StringDict_h>::make(err, out); },

			&HandleFactory<fpdemo_StringDict_h>::release,

			[](fp_ErrorMessage err,
			   fpdemo_StringDict_h handle,
			   fpdemo_String_h key,
			   fpdemo_String_h value)
			{
				using feltplugin::wrap_exception;
				return wrap_exception(
					err,
					[handle, key, value]
					{
						String const & key_str = *HandleFactory<fpdemo_String_h>::convert(key);
						String const & value_str = *HandleFactory<fpdemo_String_h>::convert(value);
						StringDict & dict = *HandleFactory<fpdemo_StringDict_h>::convert(handle);

						dict.insert({key_str, value_str});
					});
			},

			[](fp_ErrorMessage err,
			   fpdemo_String_h * out,
			   fpdemo_StringDict_h handle,
			   fpdemo_String_h key)
			{
				using feltplugin::wrap_exception;
				return wrap_exception(
					err,
					[out, handle, key]
					{
						String const & key_str = *HandleFactory<fpdemo_String_h>::convert(key);
						StringDict const & dict =
							*HandleFactory<fpdemo_StringDict_h>::convert(handle);

						std::string value = dict.at(key_str);

						*out = HandleFactory<fpdemo_String_h>::make(std::move(value));
					});
			}};
	}
}
