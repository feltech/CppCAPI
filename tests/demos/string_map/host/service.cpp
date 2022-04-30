// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include "service.hpp"

#include <feltpluginsystem-demo-string_map/interface.h>

#include "host_export.h"
#include "plugin_definition.hpp"

namespace
{
using String = feltpluginsystemdemohost::service::String;
using StringView = feltpluginsystemdemohost::service::StringView;
using StringDict = feltpluginsystemdemohost::service::StringDict;
}  // namespace

extern "C"
{
	using Plugin = feltpluginsystemdemohost::Plugin;

	// String

	using feltpluginsystemdemohost::service::String;
	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fpdemo_String_s fpdemo_String_suite()
	{
		using SuiteDecorator = Plugin::SuiteDecorator<fpdemo_String_h>;
		using HandleManager = Plugin::HandleManager<fpdemo_String_h>;

		return {
			.create = &HandleManager::make,

			.release = &HandleManager::release,

			.assign_cstr =
				SuiteDecorator::decorate([](String & self, char const * str) { self = str; }),

			.assign_StringView =
				SuiteDecorator::decorate([](String & self, StringView const & str) { self = str; }),

			.c_str = SuiteDecorator::decorate(SuiteDecorator::mem_fn_ptr<&String::c_str>),
			// clang-format off
/*
			// Overloaded method requires a static_cast. However:
			// The following does not work because https://stackoverflow.com/a/51922982/535103
			.at = SuiteDecorator::decorate(
				SuiteDecorator::mem_fn_ptr<static_cast<char const & (String::*)(size_t) const>(&String::at)>),
			// Instead we could use:
			.at = SuiteDecorator::decorate(
				SuiteDecorator::mem_fn_ptr<static_cast<char const & (std::basic_string<char>::*)(size_t) const>(&String::at)>),
*/
			// clang-format on
			// Lambda is more concise in this case due to overloaded `at`.
			.at =
				SuiteDecorator::decorate([](String const & self, size_t n) { return self.at(n); })};
	}

	// StringView

	using feltpluginsystemdemohost::service::StringView;

	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fpdemo_StringView_s fpdemo_StringView_suite()
	{
		using Decorator = Plugin::SuiteDecorator<fpdemo_StringView_h>;
		return {
			.data = Decorator::decorate(Decorator::mem_fn_ptr<&StringView::data>),

			.size = Decorator::decorate(Decorator::mem_fn_ptr<&StringView::size>)};
	}

	// StringDict

	FELTPLUGINSYSTEM_DEMO_HOST_EXPORT fpdemo_StringDict_s fpdemo_StringDict_suite()
	{
		using Decorator = Plugin::SuiteDecorator<fpdemo_StringDict_h>;
		using Converter = Plugin::HandleManager<fpdemo_StringDict_h>;

		return {
			.create = &Converter::make,

			.release = &Converter::release,

			.insert = Decorator::decorate(
				[](feltplugin::SharedPtr<StringDict> & self, String key, String value)
				{ self->insert_or_assign(std::move(key), std::move(value)); }),

			.at = Decorator::decorate([](feltplugin::SharedPtr<StringDict> const & self,
										 String const & key) { return self->at(key); })

		};
	}
}
