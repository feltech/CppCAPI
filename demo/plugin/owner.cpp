#include "owner.hpp"

#include <iostream>
#include "client.hpp"

#include <feltplugindemo/interface.h>
#include <feltplugin/errors.hpp>
#include <feltplugin/owner/handle_factory.hpp>
#include "feltplugindemoplugin_export.h"

namespace feltplugindemoplugin::owner
{

Plugin::Plugin(client::StringDict dict) : dict_{std::move(dict)} {}

void Plugin::update_dict(client::String key)
{
	dict_.insert(key, "valuefromplugin");
}
}  // namespace feltplugindemoplugin::owner

extern "C"
{
	using feltplugindemoplugin::client::String;
	using feltplugindemoplugin::client::StringDict;
	using feltplugindemoplugin::owner::HandleFactory;

	// Plugin

	FELTPLUGINDEMOPLUGIN_EXPORT fpdemo_Plugin_s fpdemo_Plugin_suite()
	{
		return {
			.create = &HandleFactory<fpdemo_Plugin_h>::make,

			.release = &HandleFactory<fpdemo_Plugin_h>::release,

			.update_dict = [](fp_ErrorMessage err, fpdemo_Plugin_h handle, fpdemo_String_h hkey)
			{
				return HandleFactory<fpdemo_Plugin_h>::mem_fn(
					[](auto & self, auto const& key) { self.update_dict(key); },
					err,
					handle,
					hkey);
			}};
	}
}
