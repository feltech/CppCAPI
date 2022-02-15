#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <feltplugin/handle_map.hpp>

#include <feltplugindemo/handles.h>
#include "feltplugin/owner/handle_factory.hpp"

namespace feltplugindemohost
{
using String = std::string;
using StringView = std::string_view;
using StringDict = std::unordered_map<String, String>;

// clang-format off
using HandleMap = feltplugin::HandleMap<
	feltplugin::HandleTraits<fp_String_h, feltplugin::UniquePtr<String>, String>,
	feltplugin::HandleTraits<fp_StringDict_h, feltplugin::SharedPtr<StringDict>, StringDict>
>;
// clang-format on

template <class Class>
using HandleFactory = feltplugin::owner::HandleFactory<Class, HandleMap>;
}
