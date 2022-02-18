#pragma once

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>

#include <feltplugindemo/interface.h>

#include "handle_map.hpp"

namespace feltplugindemoplugin::client
{
struct StringView : HandleAdapter<fpdemo_StringView_h>
{
	using Base::HandleAdapter;

	[[nodiscard]] char const * data() const;
	[[nodiscard]] size_t size() const;

	explicit operator std::string_view() const;
};

struct String : HandleAdapter<fpdemo_String_h>
{
	using Base::HandleAdapter;

	String(StringView const & str);
	String(std::string const & str);
	String(char const * local);

	[[nodiscard]] char const * c_str() const;
	[[nodiscard]] char at(int n) const;

	explicit operator std::string() const;
};


struct StringDict : HandleAdapter<fpdemo_StringDict_h>
{
	using Base::HandleAdapter;

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);
};
}  // namespace feltplugindemoplugin::client
