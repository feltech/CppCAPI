// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once
#include <stdexcept>

#include <cppcapi/plugin_definition.hpp>

#include <cppcapi-demo-string_map/interface.h>

namespace cppcapidemohost
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

using Plugin = cppcapi::PluginDefinition<
	// Service
	cppcapi::service::HandleMap<
		// StringView
		cppcapi::service::HandleTraits<
			fpdemo_StringView_h,
			service::StringView,
			cppcapi::service::HandleOwnershipTag::OwnedByService>,

		// String
		cppcapi::service::HandleTraits<
			fpdemo_String_h,
			service::String,
			cppcapi::service::HandleOwnershipTag::OwnedByClient>,

		// StringDict
		cppcapi::service::HandleTraits<
			fpdemo_StringDict_h,
			service::StringDict,
			cppcapi::service::HandleOwnershipTag::Shared>>,

	// Client
	cppcapi::client::HandleMap<
		// Worker
		cppcapi::client::HandleTraits<fpdemo_Worker_h, fpdemo_Worker_s, client::Worker>>,

	// Errors
	cppcapi::ErrorMap<
		// Out of range - e.g. map.at(...).
		cppcapi::ErrorTraits<std::out_of_range, 100>,

		// Invalid argument.
		cppcapi::ErrorTraits<std::invalid_argument, 101>>>;

}  // namespace cppcapidemohost