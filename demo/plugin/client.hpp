#pragma once

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemoplugin::client
{

// clang-format off
using HandleMap = feltplugin::client::HandleMap<
    // String.
	feltplugin::client::HandleTraits<
	    fpdemo_String_h, fp_String_s, &fpdemo_String_suite, struct String>,
	// StringDict
	feltplugin::client::HandleTraits<
	    fpdemo_StringDict_h, fp_StringDict_s, &fpdemo_StringDict_suite, struct StringDict>
>;
// clang-format on

template <class THandle>
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap>;

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
