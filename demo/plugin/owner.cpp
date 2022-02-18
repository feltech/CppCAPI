#include "owner.hpp"

#include <iostream>
#include "client.hpp"

#include <feltplugindemo/interface.h>
#include <feltplugin/owner/handle_factory.hpp>
#include "feltplugindemoplugin_export.h"

namespace feltplugindemoplugin::owner
{

Worker::Worker(client::StringDict dict) : dict_{std::move(dict)} {}

void Worker::update_dict(client::String const& key)
{
	try
	{
		auto const& value = dict_.at("expectedtoexist");
		dict_.insert(key, std::string{value} + " ok");
	}
	catch (std::exception const& ex)
	{
		std::cerr << "Error from host caught in plugin: " << ex.what() << "\n";
		dict_.insert(key, "error");
		throw std::runtime_error{"Couldn't find key expectedtoexist"};
	}
}
}  // namespace feltplugindemoplugin::owner

extern "C"
{
using feltplugindemoplugin::owner::Worker;
	using feltplugindemoplugin::client::StringView;
using feltplugindemoplugin::client::String;
	using feltplugindemoplugin::owner::HandleFactory;

	// Plugin

	FELTPLUGINDEMOPLUGIN_EXPORT fpdemo_Worker_s fpdemo_Worker_suite()
	{
		return {
			.create = &HandleFactory<fpdemo_Worker_h>::make,

			.release = &HandleFactory<fpdemo_Worker_h>::release,

			.update_dict = [](fp_ErrorMessage err, fpdemo_Worker_h handle, fpdemo_StringView_h hkey)
			{
				return HandleFactory<fpdemo_Worker_h>::mem_fn(
					[](Worker & self, StringView const& key) { self.update_dict(key); },
					err,
					handle,
					hkey);
			}};
	}
}
