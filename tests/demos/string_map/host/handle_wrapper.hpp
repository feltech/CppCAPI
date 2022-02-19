#pragma once
#include <stdexcept>

#include "feltpluginsystem/handle_wrapper.hpp"

#include <feltpluginsystem-demo-string_map/interface.h>

namespace feltplugindemohost
{
namespace client
{
class Worker;
}  // namespace client

namespace service
{
class StringView;
class String;
class StringDict;
}  // namespace service

using HandleWrapper = feltplugin::HandleWrapper<
	// Service
	feltplugin::service::HandleMap<
		// StringView
		feltplugin::service::HandleTraits<
			fpdemo_StringView_h,
			service::StringView,
			feltplugin::service::HandlePtrTag::OwnedByService>,

		// String
		feltplugin::service::HandleTraits<
			fpdemo_String_h,
			service::String,
			feltplugin::service::HandlePtrTag::OwnedByClient>,

		// StringDict
		feltplugin::service::HandleTraits<
			fpdemo_StringDict_h,
			service::StringDict,
			feltplugin::service::HandlePtrTag::Shared>>,

	// Client
	feltplugin::client::HandleMap<
		// Worker
		feltplugin::client::HandleTraits<fpdemo_Worker_h, fpdemo_Worker_s, client::Worker>>,

	// Errors
	feltplugin::ErrorMap<
		// Out of range - e.g. map.at(...).
		feltplugin::ErrorTraits<std::out_of_range, 100>,

		// Invalid argument.
		feltplugin::ErrorTraits<std::invalid_argument, 101>>>;

}  // namespace feltplugindemohost