#pragma once

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemoplugin::client
{
using feltplugin::client::HandleTraits;

using HandleMap = feltplugin::client::HandleMap<
	// String.
	HandleTraits<fpdemo_String_h, fpdemo_String_s, struct String, &fpdemo_String_suite>,

	// StringView.
	HandleTraits<
		fpdemo_StringView_h,
		fpdemo_StringView_s,
		struct StringView,
		&fpdemo_StringView_suite>,

	// StringDict
	HandleTraits<
		fpdemo_StringDict_h,
		fp_StringDict_s,
		struct StringDict,
		&fpdemo_StringDict_suite>>;

template <class THandle>
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap>;


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
