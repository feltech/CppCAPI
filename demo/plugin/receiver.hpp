#pragma once

#include <feltplugin/receiver/handle_adaptor.hpp>
#include <feltplugin/receiver/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemoplugin
{

// clang-format off
using HandleMap = feltplugin::receiver::HandleMap<
    // String.
	feltplugin::receiver::HandleTraits<
	    fpdemo_String_h, fp_String_s, &fp_String_suite, struct String>,
	// StringDict
	feltplugin::receiver::HandleTraits<
	    fpdemo_StringDict_h, fp_StringDict_s, &fp_StringDict_suite, struct StringDict>
>;
// clang-format on

template <class THandle>
using HandleAdapter = feltplugin::receiver::HandleAdapter<THandle, HandleMap>;

struct String : HandleAdapter<fpdemo_String_h>
{
	using Base::HandleAdapter;

	String(std::string const & local);
	String(char const * local);

	[[nodiscard]] char const * c_str() const;

	explicit operator std::string() const;
};

struct StringDict : HandleAdapter<fpdemo_StringDict_h>
{
	using Base::HandleAdapter;

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);
};
}  // namespace feltplugindemoplugin
