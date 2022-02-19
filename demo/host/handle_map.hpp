#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>
#include <feltplugin/service/handle_factory.hpp>
#include <feltplugin/service/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemohost::client
{
using HandleMap = feltplugin::client::HandleMap<
	// Worker
	feltplugin::client::HandleTraits<fpdemo_Worker_h, fpdemo_Worker_s, struct Worker>>;

template <class THandle>
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap>;
}  // namespace feltplugindemohost::client

namespace feltplugindemohost::service
{
using feltplugin::service::HandlePtrTag;
using HandleMap = feltplugin::service::HandleMap<
	// StringView
	feltplugin::service::
		HandleTraits<fpdemo_StringView_h, class StringView, HandlePtrTag::OwnedByService>,

	// String
	feltplugin::service::HandleTraits<fpdemo_String_h, class String, HandlePtrTag::OwnedByClient>,

	// StringDict
	feltplugin::service::HandleTraits<fpdemo_StringDict_h, class StringDict, HandlePtrTag::Shared>>;

template <class THandle>
using HandleFactory = feltplugin::service::HandleFactory<THandle, HandleMap, client::HandleMap>;
}  // namespace feltplugindemohost::service
