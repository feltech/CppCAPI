#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "feltplugin/owner/handle_map.hpp"

#include <feltplugindemo/interface.h>
#include "feltplugin/owner/handle_factory.hpp"

namespace feltplugindemohost::owner
{
using String = std::string;
using StringView = std::string_view;
using StringDict = std::unordered_map<String, String>;

using feltplugin::owner::HandlePtrTag;

// clang-format off
using HandleMap = feltplugin::owner::HandleMap<
	feltplugin::owner::HandleTraits<
		fpdemo_StringView_h, StringView, HandlePtrTag::OwnedByOwner>,
	feltplugin::owner::HandleTraits<
	    fpdemo_String_h, String, HandlePtrTag::OwnedByClient>,
	feltplugin::owner::HandleTraits<
	    fpdemo_StringDict_h, StringDict, HandlePtrTag::Shared>
>;
// clang-format on

template <class THandle>
using HandleFactory = feltplugin::owner::HandleFactory<THandle, HandleMap>;
}  // namespace feltplugindemohost::owner
