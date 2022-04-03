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
		using Decorator = HandleWrapper::Decorator<fpdemo_String_h>;
		using Converter = HandleWrapper::Converter<fpdemo_String_h>;

		return {
			.create = &Converter::make,

			.release = &Converter::release,

			.assign_cstr =
				[](fp_ErrorMessage err, fpdemo_String_h hself, char const * cstr) {
				return Decorator::mem_fn(
					[](String & self, char const * str) { self = str; }, err, hself, cstr);
			},

			.assign_StringView =
				[](fp_ErrorMessage err, fpdemo_String_h hself, fpdemo_StringView_h hstr)
			{
				return Decorator::mem_fn(
					[](String & self, StringView const & str) { self = str; }, err, hself, hstr);
			},

			.c_str = Decorator::decorate(Decorator::mem_fn_ptr<&String::c_str>),
			// clang-format off
/*
			// Overloaded method requires a static_cast. However:
			// The following does not work because https://stackoverflow.com/a/51922982/535103
			.at = Decorator::decorate(
				Decorator::mem_fn_ptr<static_cast<char const & (String::*)(size_t) const>(&String::at)>),
			// Instead we could use:
			.at = Decorator::decorate(
				Decorator::mem_fn_ptr<static_cast<char const & (std::basic_string<char>::*)(size_t) const>(&String::at)>),
*/
			// clang-format on
			// Lambda is more concise in this case due to overloaded `at`.
			.at = Decorator::decorate([](String const & self, size_t n) { return self.at(n); })};
	}

	// StringView

	using feltpluginsystemdemohost::service::StringView;

	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fpdemo_StringView_s fpdemo_StringView_suite()
	{
		using Decorator = HandleWrapper::Decorator<fpdemo_StringView_h>;
		return {
			.data =
				[](fpdemo_StringView_h handle) {
					return Decorator::mem_fn(
						[](StringView const & self) { return self.data(); }, handle);
				},

			.size =
				[](fpdemo_StringView_h handle) {
					return Decorator::mem_fn(
						[](StringView const & self) { return self.size(); }, handle);
				}};
	}

	// StringDict

	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fp_StringDict_s fpdemo_StringDict_suite()
	{
		using Decorator = HandleWrapper::Decorator<fpdemo_StringDict_h>;
		using Converter = HandleWrapper::Converter<fpdemo_StringDict_h>;

		return {
			.create = &Converter::make,

			.release = &Converter::release,

			.insert =
				[](fp_ErrorMessage err,
				   fpdemo_StringDict_h handle,
				   fpdemo_String_h key,
				   fpdemo_String_h value)
			{
				return Decorator::mem_fn(
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
				return Decorator::mem_fn(
					[](StringDict const & self, String const & key) { return self.at(key); },
					err,
					out,
					handle,
					key);
			}};
	}
}
