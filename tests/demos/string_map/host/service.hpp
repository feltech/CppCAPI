// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace feltpluginsystemdemohost::service
{
struct String : std::string
{
	using std::string::string;
	using std::string::operator=;
};
}  // namespace feltpluginsystemdemohost::service

template <>
struct std::hash<feltpluginsystemdemohost::service::String> : std::hash<std::string>
{
};

namespace feltpluginsystemdemohost::service
{
struct StringView : std::string_view
{
	using std::string_view::basic_string_view;
	using std::string_view::operator=;
};

struct StringDict : std::unordered_map<String, String>
{
	using Base = std::unordered_map<String, String>;
	using Base::unordered_map;
	using Base::operator=;
};
}  // namespace feltpluginsystemdemohost::service