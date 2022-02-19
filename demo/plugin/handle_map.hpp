#pragma once

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>
#include <feltplugin/error_map.hpp>
#include <feltplugin/service/handle_factory.hpp>
#include <feltplugin/service/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemoplugin
{
using ErrorMap = feltplugin::ErrorMap<
	// Out of range - e.g. map.at(...).
	feltplugin::ErrorTraits<std::out_of_range, 100>,

	// Invalid argument.
	feltplugin::ErrorTraits<std::invalid_argument, 101>>;

namespace client
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
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap, ErrorMap>;
}  // namespace client

namespace service
{
using feltplugin::service::HandlePtrTag;

using HandleMap = feltplugin::service::HandleMap<
	// Worker
	feltplugin::service::HandleTraits<fpdemo_Worker_h, class Worker, HandlePtrTag::OwnedByClient>>;

template <class Class>
using HandleFactory =
	feltplugin::service::HandleFactory<Class, HandleMap, client::HandleMap, ErrorMap>;
}  // namespace service
}  // namespace feltplugindemoplugin