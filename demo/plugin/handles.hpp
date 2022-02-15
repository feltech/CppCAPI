#pragma once

#include "feltplugin/owner/handle_map.hpp"
#include <feltplugin/receiver/handle_adaptor.hpp>

#include <feltplugindemo/handles.h>

namespace feltplugindemoplugin
{

using StringTraits =
	feltplugin::receiver::HandleTraits<fp_String_h, fp_String_s, &fp_String_suite, struct String>;

struct String : feltplugin::receiver::HandleAdapter<StringTraits>
{
	using Base::HandleAdapter;

	String(std::string const & local);
	String(char const * local);

	[[nodiscard]] char const * c_str() const;

	explicit operator std::string() const;
};

using StringDictTraits = feltplugin::receiver::
	HandleTraits<fp_StringDict_h, fp_StringDict_s, &fp_StringDict_suite, struct StringDict>;

struct StringDict : feltplugin::receiver::HandleAdapter<StringDictTraits>
{
	using Base::HandleAdapter;

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);
};
}  // namespace feltplugindemoplugin
