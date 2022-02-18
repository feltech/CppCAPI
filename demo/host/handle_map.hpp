#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>
#include <feltplugin/owner/handle_factory.hpp>
#include <feltplugin/owner/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemohost::client
{
using HandleMap = feltplugin::client::HandleMap<
	// Worker
	feltplugin::client::HandleTraits<fpdemo_Worker_h, fpdemo_Worker_s, struct Worker>>;

template <class THandle>
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap>;
}  // namespace feltplugindemohost::client

namespace feltplugindemohost::owner
{
using feltplugin::owner::HandlePtrTag;
using HandleMap = feltplugin::owner::HandleMap<
	// StringView
	feltplugin::owner::
		HandleTraits<fpdemo_StringView_h, class StringView, HandlePtrTag::OwnedByOwner>,

	// String
	feltplugin::owner::HandleTraits<fpdemo_String_h, class String, HandlePtrTag::OwnedByClient>,

	// StringDict
	feltplugin::owner::HandleTraits<fpdemo_StringDict_h, class StringDict, HandlePtrTag::Shared>>;

template <class THandle>
using HandleFactory = feltplugin::owner::HandleFactory<THandle, HandleMap, client::HandleMap>;
}  // namespace feltplugindemohost::owner
