// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once
#include <string>

#include <feltpluginsystem/client/suite_adaptor.hpp>
#include <feltpluginsystem/client/handle_map.hpp>

#include <feltpluginsystem-demo-string_map/interface.h>

#include "handle_wrapper.hpp"

namespace feltpluginsystemdemoplugin::client
{
struct StringView : Plugin::SuiteAdapter<fpdemo_StringView_h>
{
	using Base::SuiteAdapter;

	[[nodiscard]] char const * data() const;
	[[nodiscard]] size_t size() const;

	explicit operator std::string_view() const;
};

struct String : Plugin::SuiteAdapter<fpdemo_String_h>
{
	using Base::SuiteAdapter;

	explicit String(StringView const & str);
	explicit String(std::string const & str);
	explicit String(char const * str);

	[[nodiscard]] char const * c_str() const;
	[[nodiscard]] char at(int n) const;

	explicit operator std::string() const;
};

struct StringDict : Plugin::SuiteAdapter<fpdemo_StringDict_h>
{
	using Base::SuiteAdapter;

	StringDict();

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);
};
}  // namespace feltpluginsystemdemoplugin::client
