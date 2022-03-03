// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include "service.hpp"

#include <feltpluginsystem-demo-string_map/interface.h>

#include "handle_wrapper.hpp"
#include "host_export.h"

namespace
{
using String = feltpluginsystemdemohost::service::String;
using StringView = feltpluginsystemdemohost::service::StringView;
using StringDict = feltpluginsystemdemohost::service::StringDict;

struct CString
{
	static auto at(String const & self, size_t n)
	{
		return self.at(n);
	};

	static auto c_str(String const & self)
	{
		return self.c_str();
	}
	static auto assign_cstr(String & self, char const * str)
	{
		self = str;
	}

	static auto assign_StringView(String & self, StringView const & str)
	{
		self = str;
	}
};
}  // namespace

extern "C"
{
	using HandleWrapper = feltpluginsystemdemohost::HandleWrapper;

	// String

	using feltpluginsystemdemohost::service::String;
	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fpdemo_String_s fpdemo_String_suite()
	{
		using Factory = HandleWrapper::Factory<fpdemo_String_h>;

		return {
			.create = &Factory::make,
			.release = &Factory::release,
			.assign_cstr = &Factory::mem_fn<CString::assign_cstr>,
			.assign_StringView = &Factory::mem_fn<CString::assign_StringView>,
			.c_str = &Factory::mem_fn<CString::c_str>,
			.at = &Factory::mem_fn<CString::at>};
	}

	// StringView

	using feltpluginsystemdemohost::service::StringView;

	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fpdemo_StringView_s fpdemo_StringView_suite()
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

	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fp_StringDict_s fpdemo_StringDict_suite()
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
