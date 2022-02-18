
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
		plugin_loader.load_symbol<feltplugindemohost::client::Worker::SuiteFactory>(
			"fpdemo_Worker_suite");
	feltplugindemohost::client::Worker plugin{fpdemo_Plugin_suite, dict};

	try
	{
		plugin.update_dict("keyfromhost");
	} catch (std::exception const& ex)
	{
		std::cerr << "Error from plugin caught in host: " << ex.what() << "\n";
	}
	std::cerr << std::flush;

	for (auto [k, v] : *dict) std::cout << k << " = " << v << std::endl;

	std::cout << "Try again:" << std::endl;

	dict->insert({"expectedtoexist", "valuefromhost"});

	plugin.update_dict("keyfromhost");

	for (auto [k, v] : *dict) std::cout << k << " = " << v << std::endl;
}
}  // namespace feltplugindemohost

int main()
{
	feltplugindemohost::execute();

	return 0;
}