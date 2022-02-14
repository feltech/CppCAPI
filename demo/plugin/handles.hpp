#pragma once

#include <feltplugin/handle_map.hpp>
#include <feltplugin/plugin/handle_adaptor.hpp>

#include <feltplugindemo/handles.h>

namespace feltplugindemoplugin
{
// clang-format off
using HandleMap = feltplugin::HandleMap<
	fp_String_h,		struct String,
	fp_StringDict_h,	struct StringDict
>;
// clang-format on


template <class Handle>
struct HandleTraits{};

template <>
struct HandleTraits<fp_String_h>
{
	using Handle = fp_String_h;
	using Suite = fp_String_s;
	using Class = String;
	static constexpr auto get_suite = &fp_String_suite;
};

template <>
struct HandleTraits<fp_StringDict_h>
{
	using Handle = fp_StringDict_h;
	using Suite = fp_StringDict_s;
	using Class = StringDict;
	static constexpr auto get_suite = &fp_StringDict_suite;
};

struct String : private feltplugin::plugin::HandleAdapter<HandleTraits<fp_String_h>>
{
	using Base = HandleAdapter<HandleTraits<fp_String_h>>;

	using Base::HandleAdapter;
	using Base::operator Handle;

	String(std::string const & local);
	String(char const* local);

	[[nodiscard]] char const * c_str() const;

	explicit operator std::string() const;

	~String();
};

struct StringDict : private feltplugin::plugin::HandleAdapter<HandleTraits<fp_StringDict_h>>
{
	using Base = HandleAdapter<HandleTraits<fp_StringDict_h>>;
	using Base::HandleAdapter;
	using Base::operator Handle;

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);

	~StringDict();
};
}  // namespace feltplugindemoplugin
