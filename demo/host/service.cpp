#include "service.hpp"
#include "feltpluginsystemdemohost_export.h"

#include <feltpluginsystemdemo/interface.h>

#include "handle_wrapper.hpp"

extern "C"
{
	using HandleWrapper = feltplugindemohost::HandleWrapper;
	using String = feltplugindemohost::service::String;
	using StringView = feltplugindemohost::service::StringView;
	using StringDict = feltplugindemohost::service::StringDict;

	// String

	using feltplugindemohost::service::String;
	FELTPLUGINSYSTEM_DEMO_HOST_LIB_EXPORT fpdemo_String_s fpdemo_String_suite()
	{
		using Factory = HandleWrapper::Factory<fpdemo_String_h>;
		return {
			.create = &Factory::make,

			.release = &Factory::release,

			.assign_cstr =
				[](fp_ErrorMessage err, fpdemo_String_h hself, char const * cstr) {
					return Factory::mem_fn(
						[](String & self, char const * str) { self = str; }, err, hself, cstr);
				},

			.assign_StringView =
				[](fp_ErrorMessage err, fpdemo_String_h hself, fpdemo_StringView_h hstr)
			{
				return Factory::mem_fn(
					[](String & self, StringView const & str) { self = str; }, err, hself, hstr);
			},

			.c_str = [](fpdemo_String_h handle)
			{ return Factory::mem_fn([](String const & self) { return self.c_str(); }, handle); },

			.at =
				[](fp_ErrorMessage err, char * out, fpdemo_String_h handle, int n)
			{
				return Factory::mem_fn(
					[](String const & self, int n) { return self.at(n); }, err, out, handle, n);
			}};
	}

	// StringView

	using feltplugindemohost::service::StringView;

	FELTPLUGINSYSTEM_DEMO_HOST_LIB_EXPORT fpdemo_StringView_s fpdemo_StringView_suite()
	{
		using Factory = HandleWrapper::Factory<fpdemo_StringView_h>;
		return {
			.data =
				[](fpdemo_StringView_h handle)
			{
				return Factory::mem_fn([](StringView const & self) { return self.data(); }, handle);
			},

			.size =
				[](fpdemo_StringView_h handle)
			{
				return Factory::mem_fn([](StringView const & self) { return self.size(); }, handle);
			}};
	}

	// StringDict

	FELTPLUGINSYSTEM_DEMO_HOST_LIB_EXPORT fp_StringDict_s fpdemo_StringDict_suite()
	{
		using Factory = HandleWrapper::Factory<fpdemo_StringDict_h>;
		return {
			.create = &Factory::make,

			.release = &Factory::release,

			.insert =
				[](fp_ErrorMessage err,
				   fpdemo_StringDict_h handle,
				   fpdemo_String_h key,
				   fpdemo_String_h value)
			{
				return Factory::mem_fn(
					[](StringDict & self, String key, String value)
					{ return self.insert_or_assign(std::move(key), std::move(value)); },
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
				return Factory::mem_fn(
					[](StringDict const & self, String const & key) { return self.at(key); },
					err,
					out,
					handle,
					key);
			}};
	}
}
