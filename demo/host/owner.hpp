#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace feltplugindemohost::owner
{
struct String : std::string
{
	using std::string::string;
	using std::string::operator=;
};
}  // namespace feltplugindemohost::owner

template <>
struct std::hash<feltplugindemohost::owner::String> : std::hash<std::string>
{
};

namespace feltplugindemohost::owner
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
}  // namespace feltplugindemohost::owner