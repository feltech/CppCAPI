#include "handles.hpp"

namespace feltplugindemoplugin
{
String::String(std::string const & local)
	: HandleAdapter{Base::create(&fp_String_create, local.c_str())}
{
}

String::String(char const * local)
	: HandleAdapter{Base::create(&fp_String_create, local)}
{
}

[[nodiscard]] char const * String::c_str() const
{
	return fp_String_cstr(handle);
}

String::operator std::string() const
{
	return std::string{c_str()};
}

String::~String()
{
	fp_String_release(handle);
}

StringView::StringView(std::string_view const & local)
	: HandleAdapter{Base::create(&fp_StringView_create, local.data(), local.size())}
{
}

[[nodiscard]] char const * StringView::data() const
{
	return fp_StringView_data(handle);
}

[[nodiscard]] std::size_t StringView::size() const
{
	return fp_StringView_size(handle);
}

StringView::operator std::string_view() const
{
	return std::string_view{data(), size()};
}

StringView::~StringView()
{
	fp_StringView_release(handle);
}

String StringDict::at(String const & key)
{
	return String{call(&fp_StringDict_at, static_cast<fp_String>(key))};
}

void StringDict::insert(String const & key, String const & value)
{
	call(&fp_StringDict_insert, static_cast<fp_String>(key), static_cast<fp_String>(value));
}

StringDict::~StringDict()
{
	fp_StringDict_release(handle);
}
}  // namespace feltplugindemoplugin
