#pragma once

#include <feltplugin/handle_map.hpp>
#include <feltplugin/plugin/handle_adaptor.hpp>

#include <feltplugindemo/handles.h>

namespace feltplugindemoplugin
{
// clang-format off
using HandleMap = feltplugin::HandleMap<
	fp_String,		struct String,
	fp_StringView,	struct StringView,
	fp_StringDict,	struct StringDict
>;
// clang-format on

struct String : private feltplugin::plugin::HandleAdapter<fp_String>
{
	using Handle = fp_String;
	using Base = HandleAdapter<Handle>;

	using Base::HandleAdapter;
	using Base::operator Handle;

	String(std::string const & local);
	String(char const* local);

	[[nodiscard]] char const * c_str() const;

	explicit operator std::string() const;

	~String();
};

struct StringView : private feltplugin::plugin::HandleAdapter<fp_StringView>
{
	using Handle = fp_StringView;
	using Base = HandleAdapter<Handle>;

	using Base::HandleAdapter;
	using Base::operator Handle;

	explicit StringView(std::string_view const & local);

	[[nodiscard]] char const * data() const;

	[[nodiscard]] std::size_t size() const;

	explicit operator std::string_view() const;

	~StringView();
};

struct StringDict : private feltplugin::plugin::HandleAdapter<fp_StringDict>
{
	using Handle = fp_StringDict;
	using Base = HandleAdapter<Handle>;

	using Base::HandleAdapter;
	using Base::operator Handle;

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);

	~StringDict();
};
}  // namespace feltplugindemoplugin
