// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once
#include <stdexcept>

#include <cppcapi/plugin_definition.hpp>

#include <cppcapi-demo-string_map/interface.h>

namespace cppcapidemoplugin
{
namespace service
{
struct Worker;
}

namespace client
{
struct String;
struct StringView;
struct StringDict;
}  // namespace client

using Plugin = cppcapi::PluginDefinition<
	// Service
	cppcapi::service::HandleMap<
		// Worker
		cppcapi::service::HandleTraits<
			cppcapidemo_Worker_h,
			cppcapidemoplugin::service::Worker,
			cppcapi::service::HandleOwnershipTag::OwnedByClient>>,

	// Client
	cppcapi::client::HandleMap<
		// String.
		cppcapi::client::HandleTraits<
			cppcapidemo_String_h,
			cppcapidemo_String_s,
			cppcapidemoplugin::client::String,
			&cppcapidemo_String_suite>,

		// StringView.
		cppcapi::client::HandleTraits<
			cppcapidemo_StringView_h,
			cppcapidemo_StringView_s,
			cppcapidemoplugin::client::StringView,
			&cppcapidemo_StringView_suite>,

		// StringDict
		cppcapi::client::HandleTraits<
			cppcapidemo_StringDict_h,
			cppcapidemo_StringDict_s,
			cppcapidemoplugin::client::StringDict,
			&cppcapidemo_StringDict_suite>>,

	// Errors
	cppcapi::ErrorMap<
		// Invalid argument.
		cppcapi::ErrorTraits<std::invalid_argument, 101>,

		// Out of range - e.g. map.at(...).
		cppcapi::ErrorTraits<std::out_of_range, 100>>>;
}  // namespace cppcapidemoplugin