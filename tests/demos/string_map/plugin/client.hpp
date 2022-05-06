// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once
#include <string>

#include <cppcapi/client/handle_map.hpp>
#include <cppcapi/client/suite_adaptor.hpp>

#include <cppcapi-demo-string_map/interface.h>

#include "plugin_definition.hpp"

namespace cppcapidemoplugin::client
{
struct StringView : Plugin::SuiteAdapter<cppcapidemo_StringView_h>
{
	using Base::SuiteAdapter;

	[[nodiscard]] char const * data() const;
	[[nodiscard]] size_t size() const;

	explicit operator std::string_view() const;
};

struct String : Plugin::SuiteAdapter<cppcapidemo_String_h>
{
	using Base::SuiteAdapter;

	explicit String(StringView const & str);
	explicit String(std::string const & str);
	explicit String(char const * str);

	[[nodiscard]] char const * c_str() const;
	[[nodiscard]] char at(int n) const;

	explicit operator std::string() const;
};

struct StringDict : Plugin::SuiteAdapter<cppcapidemo_StringDict_h>
{
	using Base::SuiteAdapter;

	StringDict();

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);
};
}  // namespace cppcapidemoplugin::client
