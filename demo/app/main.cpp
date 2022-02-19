
#include <iostream>

#include <feltplugin/service/plugin_factory.hpp>

#include "../host/client.hpp"
#include "../host/service.hpp"

namespace feltplugindemohost
{
void execute()
{
	auto dict = feltplugin::make_shared<service::StringDict>(
		service::StringDict{{"key at construction", "value at construction"}});

	feltplugin::service::Plugin plugin{"./libFeltPluginDemoPlugin.so"};

	auto worker =
		plugin.load_adapter<feltplugindemohost::client::Worker>("fpdemo_Worker_suite", dict);

	try
	{
		worker.update_dict("first key from host");
	}
	catch (std::invalid_argument const & ex)
	{
		std::cerr << "Invalid argument error from plugin caught in host: " << ex.what() << "\n";
	}
	catch (std::exception const & ex)
	{
		std::cerr << "Unexpected error from plugin caught in host: " << ex.what() << "\n";
	}
	std::cerr << std::flush;

	for (auto [k, v] : *dict) std::cout << k << " = " << v << std::endl;

	std::cout << "Try again:" << std::endl;

	dict->insert({"plugin expects to exist", "value from host"});

	worker.update_dict("second key from host");

	for (auto [k, v] : *dict) std::cout << k << " = " << v << std::endl;
}
}  // namespace feltplugindemohost

int main()
{
	feltplugindemohost::execute();

	return 0;
}