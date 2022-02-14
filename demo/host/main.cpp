
#include <feltplugin/errors.hpp>
#include <feltplugin/host/plugin_factory.hpp>
#include <iostream>

#include "handles.hpp"

namespace feltplugindemohost
{
void execute()
{
	feltplugin::host::Plugin plugin{"./libFeltPluginDemoPlugin.so"};

	auto update_dict =
		plugin.load_symbol<fp_ErrorCode (*)(fp_ErrorMessage, fp_StringDict_h)>("update_dict");

	auto dict_ptr = feltplugin::make_shared<StringDict>(
		StringDict{{String{"hostkey"}, String{"hostvalue"}}});

	auto dict_handle = HandleFactory<StringDict>::create(dict_ptr);

	try
	{
		fp_ErrorMessage err;
		fp_ErrorCode code = update_dict(err, dict_handle);
		feltplugin::throw_on_error(code, err);
	}
	catch (std::exception const & ex)
	{
		std::cerr << "Exception propagated from plugin and caught within host: " << ex.what()
				  << "\n";
	}
	std::cerr << std::flush;

	for (auto [k, v] : *dict_ptr) std::cout << k << " = " << v << "\n";
}
}  // namespace feltplugindemohost

int main()
{
	feltplugindemohost::execute();
}