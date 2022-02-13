#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <feltplugin/handle_map.hpp>

#include <feltplugindemo/handles.h>
#include "feltplugin/host/handle_factory.hpp"

namespace feltplugindemohost
{
using String = std::string;
using StringView = std::string_view;
using StringDict = std::unordered_map<String, String>;

// clang-format off
using HandleMap = feltplugin::HandleMap<
	fp_String,		feltplugin::UniquePtr<String>,
	fp_StringView,	feltplugin::RawPtr<StringView>,
	fp_StringDict,	feltplugin::SharedPtr<StringDict>
>;
// clang-format on

template <class Class>
using HandleFactory = feltplugin::host::HandleFactory<Class, HandleMap>;
}
