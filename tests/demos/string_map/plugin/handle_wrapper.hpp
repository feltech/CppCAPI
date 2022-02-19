#pragma once
#include <stdexcept>

#include "feltpluginsystem/handle_wrapper.hpp"

#include <feltpluginsystem-demo-string_map/interface.h>

namespace feltplugindemoplugin
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

using HandleWrapper = feltplugin::HandleWrapper<
	// Service
	feltplugin::service::HandleMap<
		// Worker
		feltplugin::service::HandleTraits<
			fpdemo_Worker_h,
			feltplugindemoplugin::service::Worker,
			feltplugin::service::HandlePtrTag::OwnedByClient>>,

	// Client
	feltplugin::client::HandleMap<
		// String.
		feltplugin::client::HandleTraits<
			fpdemo_String_h,
			fpdemo_String_s,
			feltplugindemoplugin::client::String,
			&fpdemo_String_suite>,

		// StringView.
		feltplugin::client::HandleTraits<
			fpdemo_StringView_h,
			fpdemo_StringView_s,
			feltplugindemoplugin::client::StringView,
			&fpdemo_StringView_suite>,

		// StringDict
		feltplugin::client::HandleTraits<
			fpdemo_StringDict_h,
			fp_StringDict_s,
			feltplugindemoplugin::client::StringDict,
			&fpdemo_StringDict_suite>>,

	// Errors
	feltplugin::ErrorMap<
		// Out of range - e.g. map.at(...).
		feltplugin::ErrorTraits<std::out_of_range, 100>,

		// Invalid argument.
		feltplugin::ErrorTraits<std::invalid_argument, 101>>>;
}  // namespace feltplugindemoplugin