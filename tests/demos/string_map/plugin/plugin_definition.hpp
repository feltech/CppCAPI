// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once
#include <stdexcept>

#include <feltpluginsystem/plugin_definition.hpp>

#include <feltpluginsystem-demo-string_map/interface.h>

namespace feltpluginsystemdemoplugin
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

using Plugin = feltplugin::PluginDefinition<
	// Service
	feltplugin::service::HandleMap<
		// Worker
		feltplugin::service::HandleTraits<
			fpdemo_Worker_h,
			feltpluginsystemdemoplugin::service::Worker,
			feltplugin::service::HandleOwnershipTag::OwnedByClient>>,

	// Client
	feltplugin::client::HandleMap<
		// String.
		feltplugin::client::HandleTraits<
			fpdemo_String_h,
			fpdemo_String_s,
			feltpluginsystemdemoplugin::client::String,
			&fpdemo_String_suite>,

		// StringView.
		feltplugin::client::HandleTraits<
			fpdemo_StringView_h,
			fpdemo_StringView_s,
			feltpluginsystemdemoplugin::client::StringView,
			&fpdemo_StringView_suite>,

		// StringDict
		feltplugin::client::HandleTraits<
			fpdemo_StringDict_h,
			fp_StringDict_s,
			feltpluginsystemdemoplugin::client::StringDict,
			&fpdemo_StringDict_suite>>,

	// Errors
	feltplugin::ErrorMap<
		// Invalid argument.
		feltplugin::ErrorTraits<std::invalid_argument, 101>,

		// Out of range - e.g. map.at(...).
		feltplugin::ErrorTraits<std::out_of_range, 100>>>;
}  // namespace feltpluginsystemdemoplugin