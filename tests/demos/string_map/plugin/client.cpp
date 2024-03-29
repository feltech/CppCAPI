// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include "client.hpp"

#include <string>

namespace cppcapidemoplugin::client
{

String::String(client::StringView const & str)
{
	Base::create();
	call(suite_.assign_StringView, str);
}

String::String(char const * str)
{
	Base::create();
	call(suite_.assign_cstr, str);
}

String::String(std::string const & str)
{
	Base::create();
	call(suite_.assign_cstr, str.c_str());
}

char const * String::c_str() const
{
	return suite_.c_str(handle_);
}

char String::at(int n) const
{
	return call(suite_.at, n);
}

String::operator std::string() const
{
	return std::string{c_str()};
}

char const * StringView::data() const
{
	return call(suite_.data);
}

size_t StringView::size() const
{
	return call(suite_.size);
}

StringView::operator std::string_view() const
{
	return std::string_view{data(), size()};
}

StringDict::StringDict()
{
	Base::create();
}

String StringDict::at(String const & key)
{
	return String{call(suite_.at, key)};
}

void StringDict::insert(String const & key, String const & value)
{
	call(suite_.insert, key, value);
}
}  // namespace cppcapidemoplugin::client
