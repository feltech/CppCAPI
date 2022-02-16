#include "owner.hpp"

#include "feltplugindemoplugin_export.h"
#include "client.hpp"

#include <feltplugindemo/interface.h>
#include <feltplugin/errors.hpp>
#include <feltplugin/owner/handle_factory.hpp>
#include "feltplugin/owner/handle_map.hpp"

namespace feltplugindemoplugin::owner
{

Plugin::Plugin(client::StringDict dict) : dict_{std::move(dict)} {}

void Plugin::update_dict(client::String key)
{
	dict_.insert(key, "valuefromplugin");
}
}  // namespace feltplugindemoplugin::sender

extern "C"
{
	using feltplugindemoplugin::client::String;
	using feltplugindemoplugin::client::StringDict;
	using feltplugindemoplugin::owner::HandleFactory;

	// Plugin

	FELTPLUGINDEMOPLUGIN_EXPORT fpdemo_Plugin_s fpdemo_Plugin_suite()
	{
		return {
			[](fp_ErrorMessage err, fpdemo_Plugin_h * out, fpdemo_StringDict_h hdict)
			{
				auto dict = StringDict{hdict};
				return HandleFactory<fpdemo_Plugin_h>::make(err, out, std::move(dict));
			},

			&HandleFactory<fpdemo_Plugin_h>::release,

			[](fp_ErrorMessage err, fpdemo_Plugin_h handle, fpdemo_String_h key)
			{
				using feltplugin::wrap_exception;
				return wrap_exception(
					err,
					[handle, key] {
						return HandleFactory<fpdemo_Plugin_h>::convert(handle)->update_dict(
							String{key});
					});
			}};
	}
}
