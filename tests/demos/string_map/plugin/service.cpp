#include "service.hpp"

#include <iostream>

#include <feltpluginsystem-demo-string_map/interface.h>

#include <feltpluginsystem/service/handle_factory.hpp>

#include "plugin_export.h"
#include "client.hpp"

namespace feltpluginsystemdemoplugin::service
{

Worker::Worker(client::StringDict dict) : dict_{std::move(dict)} {}

void Worker::update_dict(client::String const & key)
{
	try
	{
		auto const & value = dict_.at(client::String{"plugin expects to exist"});
		dict_.insert(key, client::String{std::string{value} + " updated by plugin"});
	}
	catch (std::out_of_range const & ex)
	{
		std::cerr << "Out of range error from host caught in plugin: " << ex.what() << "\n";
		dict_.insert(key, client::String{"error from plugin"});
		throw std::invalid_argument{"Couldn't find key plugin expects to exist"};
	}
	catch (std::exception const & ex)
	{
		std::cerr << "Unexpected error from host caught in plugin: " << ex.what() << "\n";
		throw;
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
			.create = &HandleWrapper::Factory<fpdemo_Worker_h>::make,

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
