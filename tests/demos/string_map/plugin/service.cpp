// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include "service.hpp"

#include <iostream>

#include <feltpluginsystem-demo-string_map/interface.h>

#include <feltpluginsystem/service/handle_factory.hpp>

#include "client.hpp"
#include "plugin_export.h"

namespace feltpluginsystemdemoplugin::service
{

Worker::Worker(client::StringDict service_dict) : service_dict_{std::move(service_dict)} {}

/**
 * Do some pointless queries and updates on StringDicts with some exception wrangling.
 *
 * @param key Key in service dict to update.
 */
void Worker::update_dict(client::String const & key)
{
	client_dict_.insert(client::String{"plugin client key"}, client::String{"plugin client value"});
	client::String client_value = client_dict_.at(client::String{"plugin client key"});
	try
	{
		auto const & value = service_dict_.at(client::String{"plugin expects to exist"});
		service_dict_.insert(
			key,
			client::String{
				std::string{value} + " updated by plugin to " + std::string{client_value}});
	}
	catch (std::out_of_range const & ex)
	{
		std::cout << "Out of range error from host caught in plugin: " << ex.what() << std::endl;
		service_dict_.insert(key, client::String{"error from plugin"});
		throw std::invalid_argument{"Couldn't find key plugin expects to exist"};
	}
}
}  // namespace feltpluginsystemdemoplugin::service

extern "C"
{
	using feltpluginsystemdemoplugin::HandleWrapper;
	using feltpluginsystemdemoplugin::client::String;
	using feltpluginsystemdemoplugin::client::StringView;
	using feltpluginsystemdemoplugin::service::Worker;

	// Plugin

	FELTPLUGINSYSTEM_DEMO_PLUGIN_EXPORT fpdemo_Worker_s fpdemo_Worker_suite()
	{
		return {
			.create = &HandleWrapper::Factory<fpdemo_Worker_h>::ThisConverter::make,

			.release = &HandleWrapper::Factory<fpdemo_Worker_h>::release,

			.update_dict = [](fp_ErrorMessage err, fpdemo_Worker_h handle, fpdemo_StringView_h hkey)
			{
				return HandleWrapper::Factory<fpdemo_Worker_h>::mem_fn(
					[](Worker & self, StringView const & key) { self.update_dict(String{key}); },
					err,
					handle,
					hkey);
			}};
	}
}
