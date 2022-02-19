#pragma once
#include <string>

#include <feltpluginsystem/client/handle_adaptor.hpp>
#include <feltpluginsystem/client/handle_map.hpp>

#include <feltpluginsystemdemo/interface.h>

#include "handle_wrapper.hpp"

namespace feltplugindemoplugin::client
{
struct StringView : HandleWrapper::Adapter<fpdemo_StringView_h>
{
	using Base::HandleAdapter;

	[[nodiscard]] char const * data() const;
	[[nodiscard]] size_t size() const;

	explicit operator std::string_view() const;
};

struct String : HandleWrapper::Adapter<fpdemo_String_h>
{
	using Base::HandleAdapter;

	explicit String(StringView const & str);
	explicit String(std::string const & str);
	explicit String(char const * local);

	[[nodiscard]] char const * c_str() const;
	[[nodiscard]] char at(int n) const;

	explicit operator std::string() const;
};

struct StringDict : HandleWrapper::Adapter<fpdemo_StringDict_h>
{
	using Base::HandleAdapter;

	[[nodiscard]] String at(String const & key);

	void insert(String const & key, String const & value);
};
}  // namespace feltplugindemoplugin::client
