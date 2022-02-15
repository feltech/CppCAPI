#include "receiver.hpp"

namespace feltplugindemoplugin
{
String::String(std::string const & local) : String{local.c_str()} {}

String::String(char const * local)
{
	Base::create(local);
}

[[nodiscard]] char const * String::c_str() const
{
	return suite_.c_str(handle_);
}

String::operator std::string() const
{
	return std::string{c_str()};
}

String StringDict::at(String const & key)
{
	return String{call(suite_.at, static_cast<fpdemo_String_h>(key))};
}

void StringDict::insert(String const & key, String const & value)
{
	call(suite_.insert, static_cast<fpdemo_String_h>(key), static_cast<fpdemo_String_h>(value));
}

}  // namespace feltplugindemoplugin