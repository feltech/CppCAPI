
#include <feltplugin/owner/plugin_factory.hpp>
#include <iostream>

#include "client.hpp"
#include "owner.hpp"

namespace feltplugindemohost
{
void execute()
{
	auto dict = feltplugin::make_shared<owner::StringDict>(
		owner::StringDict{{"keyatconstruction", "valueatconstruction"}});

	feltplugin::owner::PluginLoader plugin_loader{"./libFeltPluginDemoPlugin.so"};

	auto const fpdemo_Plugin_suite =
		plugin_loader.load_symbol<feltplugindemohost::client::Plugin::SuiteFactory>(
			"fpdemo_Plugin_suite");
	feltplugindemohost::client::Plugin plugin{fpdemo_Plugin_suite, dict};

	plugin.update_dict("keyfromhost");

	//
	//	auto update_dict =
	//		plugin.load_symbol<fp_ErrorCode (*)(fp_ErrorMessage,
	// fpdemo_StringDict_h)>("update_dict");
	//
	//	auto dict_ptr = feltplugin::make_shared<StringDict>(
	//		StringDict{{String{"hostkey"}, String{"hostvalue"}}});
	//
	//	auto dict_handle = HandleFactory<fpdemo_StringDict_h>::create(dict_ptr);
	//
	//	try
	//	{
	//		fp_ErrorMessage err;
	//		fp_ErrorCode code = update_dict(err, dict_handle);
	//		feltplugin::throw_on_error(code, err);
	//	}
	//	catch (std::exception const & ex)
	//	{
	//		std::cerr << "Exception propagated from plugin and caught within host: " << ex.what()
	//				  << "\n";
	//	}
	//	std::cerr << std::flush;
	//
	for (auto [k, v] : *dict) std::cout << k << " = " << v << std::endl;
}
}  // namespace feltplugindemohost

int main()
{
	feltplugindemohost::execute();

	return 0;
}