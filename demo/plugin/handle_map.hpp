#pragma once

#include <feltplugin/owner/handle_factory.hpp>
#include <feltplugin/owner/handle_map.hpp>
#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>

#include <feltplugindemo/interface.h>

namespace feltplugindemoplugin::client
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
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap>;
}  // namespace feltplugindemoplugin::client

namespace feltplugindemoplugin::owner
{
using feltplugin::owner::HandlePtrTag;

using HandleMap = feltplugin::owner::HandleMap<
	// Worker
	feltplugin::owner::HandleTraits<fpdemo_Worker_h, class Worker, HandlePtrTag::OwnedByClient>>;

template <class Class>
using HandleFactory = feltplugin::owner::HandleFactory<Class, HandleMap, client::HandleMap>;
}  // namespace feltplugindemoplugin::owner
