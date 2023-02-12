// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include "service.hpp"

#include <cppcapi-demo-string_map/interface.h>

#include "host_export.h"
#include "plugin_definition.hpp"
namespace cppcapidemohost::service
{

void assign(String & self, char const * str) { self = str; }

extern "C"
{
	// String

	CPPCAPI_DEMO_HOST_EXPORT cppcapidemo_String_s cppcapidemo_String_suite()
	{
		using SuiteDecorator = Plugin::SuiteDecorator<cppcapidemo_String_h>;

		return {
			.create = &SuiteDecorator::create,

			.release = &SuiteDecorator::release,

			.assign_cstr =
				SuiteDecorator::decorate(SuiteDecorator::free_fn_ptr<&assign>),

			.assign_StringView =
				SuiteDecorator::decorate([](String & self, StringView const & str) { self = str; }),

			.c_str = SuiteDecorator::decorate<&String::c_str>(),
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

	CPPCAPI_DEMO_HOST_EXPORT cppcapidemo_StringView_s cppcapidemo_StringView_suite()
	{
		using Decorator = Plugin::SuiteDecorator<cppcapidemo_StringView_h>;
		return {
			.data = Decorator::decorate(Decorator::mem_fn_ptr<&StringView::data>),

			.size = Decorator::decorate(Decorator::mem_fn_ptr<&StringView::size>)};
	}

	// StringDict

	CPPCAPI_DEMO_HOST_EXPORT cppcapidemo_StringDict_s cppcapidemo_StringDict_suite()
	{
		using Decorator = Plugin::SuiteDecorator<cppcapidemo_StringDict_h>;

		return {
			.create = &Decorator::create,

			.release = &Decorator::release,

			.insert =
				Decorator::decorate([](StringDict & self, String key, String value)
									{ self.insert_or_assign(std::move(key), std::move(value)); }),

			.at = Decorator::decorate([](StringDict const & self, String const & key)
									  { return self.at(key); })

		};
	}
}
}  // namespace cppcapidemohost::service