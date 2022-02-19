#include "service.hpp"

#include <iostream>
#include "client.hpp"

#include <feltplugindemo/interface.h>
#include <feltplugin/service/handle_factory.hpp>
#include "feltplugindemoplugin_export.h"

namespace feltplugindemoplugin::service
{

Worker::Worker(client::StringDict dict) : dict_{std::move(dict)} {}

void Worker::update_dict(client::String const & key)
{
	try
	{
		auto const & value = dict_.at("plugin expects to exist");
		dict_.insert(key, std::string{value} + " updated by plugin");
	}
	catch (std::out_of_range const & ex)
	{
		std::cerr << "Out of range error from host caught in plugin: " << ex.what() << "\n";
		dict_.insert(key, "error from plugin");
		throw std::invalid_argument{"Couldn't find key plugin expects to exist"};
	}
	catch (std::exception const & ex)
	{
		std::cerr << "Unexpected error from host caught in plugin: " << ex.what() << "\n";
		throw;
	}
}
}  // namespace feltplugindemoplugin::service

extern "C"
{
	using feltplugindemoplugin::client::String;
	using feltplugindemoplugin::client::StringView;
	using feltplugindemoplugin::service::HandleFactory;
	using feltplugindemoplugin::service::Worker;

	// Plugin

	FELTPLUGINDEMOPLUGIN_EXPORT fpdemo_Worker_s fpdemo_Worker_suite()
	{
		return {
			.create = &HandleFactory<fpdemo_Worker_h>::make,

			.release = &HandleFactory<fpdemo_Worker_h>::release,

			.update_dict = [](fp_ErrorMessage err, fpdemo_Worker_h handle, fpdemo_StringView_h hkey)
			{
				return HandleFactory<fpdemo_Worker_h>::mem_fn(
					[](Worker & self, StringView const & key) { self.update_dict(key); },
					err,
					handle,
					hkey);
			}};
	}
}
