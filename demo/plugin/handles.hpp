#pragma once

#include <feltplugin/handle_map.hpp>
#include <feltplugin/plugin/handle_adaptor.hpp>

#include <feltplugindemo/handles.h>

namespace feltplugindemoplugin
{

using StringTraits =
	feltplugin::plugin::HandleTraits<fp_String_h, fp_String_s, &fp_String_suite, struct String>;

struct String : feltplugin::plugin::HandleAdapter<StringTraits>
{
	using Base::HandleAdapter;

	String(std::string const & local);
	String(char const * local);

	[[nodiscard]] char const * c_str() const;

	explicit operator std::string() const;
};

using StringDictTraits = feltplugin::plugin::
	HandleTraits<fp_StringDict_h, fp_StringDict_s, &fp_StringDict_suite, struct StringDict>;

struct StringDict : feltplugin::plugin::HandleAdapter<StringDictTraits>
{
	using Base::HandleAdapter;

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);
};
}  // namespace feltplugindemoplugin
