// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include "service.hpp"

#include <iostream>

#include <cppcapi-demo-string_map/interface.h>

#include "client.hpp"
#include "plugin_definition.hpp"
#include "plugin_export.h"

namespace cppcapidemoplugin::service
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

extern "C"
{
	using HandleManager = Plugin::HandleManager<cppcapidemo_Worker_h>;
	using SuiteDecorator = Plugin::SuiteDecorator<cppcapidemo_Worker_h>;

	// Plugin

	CPPCAPI_DEMO_PLUGIN_EXPORT cppcapidemo_Worker_s cppcapidemo_Worker_suite()
	{
		return {
			.create = &HandleManager::create,

			.release = &HandleManager::release,

			.update_dict =
				SuiteDecorator::decorate([](Worker & self, client::StringView const & key)
										 { self.update_dict(client::String{key}); })};
	}
}
}  // namespace cppcapidemoplugin::service
