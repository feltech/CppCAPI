#include "client.hpp"

#include <string>

namespace feltpluginsystemdemoplugin::client
{

String::String(StringView const & str)
{
	Base::create();
	call(suite_.assign_StringView, static_cast<fpdemo_StringView_h>(str));
}

String::String(char const * cstr)
{
	Base::create();
	call(suite_.assign_cstr, cstr);
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
	return suite_.data(handle_);
}

size_t StringView::size() const
{
	return suite_.size(handle_);
}

StringView::operator std::string_view() const
{
	return std::string_view{data(), size()};
}

String StringDict::at(String const & key)
{
	return String{call(suite_.at, static_cast<fpdemo_String_h>(key))};
}

void StringDict::insert(String const & key, String const & value)
{
	call(suite_.insert, static_cast<fpdemo_String_h>(key), static_cast<fpdemo_String_h>(value));
}
}  // namespace feltpluginsystemdemoplugin::client
