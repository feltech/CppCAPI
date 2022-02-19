#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>
#include <feltplugin/error_map.hpp>
#include <feltplugin/service/handle_factory.hpp>
#include <feltplugin/service/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemohost
{
using ErrorMap = feltplugin::ErrorMap<
	// Out of range - e.g. map.at(...).
	feltplugin::ErrorTraits<std::out_of_range, 100>,

	// Invalid argument.
	feltplugin::ErrorTraits<std::invalid_argument, 101>>;

namespace client
{
using HandleMap = feltplugin::client::HandleMap<
	// Worker
	feltplugin::client::HandleTraits<fpdemo_Worker_h, fpdemo_Worker_s, struct Worker>>;

template <class THandle>
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap, ErrorMap>;
}  // namespace client

namespace service
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
using HandleFactory =
	feltplugin::service::HandleFactory<THandle, HandleMap, client::HandleMap, ErrorMap>;
}  // namespace service

}  // namespace feltplugindemohost