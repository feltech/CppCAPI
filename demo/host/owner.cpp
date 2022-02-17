#include "owner.hpp"
#include "feltplugindemohostlib_export.h"

#include <feltplugindemo/interface.h>
#include <feltplugin/errors.hpp>
#include <feltplugin/owner/handle_factory.hpp>

extern "C"
{
	using feltplugindemohost::owner::HandleFactory;

	// String

	using feltplugindemohost::owner::String;

	FELTPLUGINDEMOHOSTLIB_EXPORT fpdemo_String_s fpdemo_String_suite()
	{
		return {
			.create = &HandleFactory<fpdemo_String_h>::make,

			.release = &HandleFactory<fpdemo_String_h>::release,

			.c_str =
				[](fpdemo_String_h handle)
			{
				return HandleFactory<fpdemo_String_h>::mem_fn(
					[](auto const & self) -> char const* { return self.c_str(); }, handle);
			},

			.at =
				[](fp_ErrorMessage err, char * out, fpdemo_String_h handle, int n)
			{
				return HandleFactory<fpdemo_String_h>::mem_fn(
					[](auto & self, int n) { return self.at(n); }, err, out, handle, n);
			}};
	}

	// StringDict

	using feltplugindemohost::owner::StringDict;

	FELTPLUGINDEMOHOSTLIB_EXPORT fp_StringDict_s fpdemo_StringDict_suite()
	{
		return {
			.create = &HandleFactory<fpdemo_StringDict_h>::make,

			.release = &HandleFactory<fpdemo_StringDict_h>::release,

			.insert =
				[](fp_ErrorMessage err,
				   fpdemo_StringDict_h handle,
				   fpdemo_String_h key,
				   fpdemo_String_h value)
			{
				return HandleFactory<fpdemo_StringDict_h>::mem_fn(
					[](auto & self, auto const & key, auto const & value) {
						return self.insert({key, value});
					},
					err,
					handle,
					key,
					value);
			},

			.at =
				[](fp_ErrorMessage err,
				   fpdemo_String_h * out,
				   fpdemo_StringDict_h handle,
				   fpdemo_String_h key)
			{
				return HandleFactory<fpdemo_StringDict_h>::mem_fn(
					[](auto const & self, auto const & key) { return self.at(key); },
					err,
					out,
					handle,
					key);
			}};
	}
}
