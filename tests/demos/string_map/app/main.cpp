#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <feltpluginsystem/plugin.hpp>

#include "../host/client.hpp"
#include "../host/service.hpp"

namespace feltpluginsystemdemohost
{
void execute()
{
	// Calculate plugin path.
	char const * plugin_path_var = std::getenv("FELTPLUGINSYSTEM_PLUGIN_PATH");
	if (!plugin_path_var)
		throw std::invalid_argument{"FELTPLUGINSYSTEM_PLUGIN_PATH must be set"};
	std::filesystem::path plugin_path{plugin_path_var};

	plugin_path /= "libfeltpluginsystem-demo-string_map-plugin.so";
	std::cout << "Loading plugin at " << plugin_path << std::endl;

	// Load the plugin DSO.
	feltplugin::Plugin plugin{plugin_path.c_str()};

	// Create a shared StringDict to be used by both host and plugin.
	auto dict = feltplugin::make_shared<service::StringDict>(
		service::StringDict{{"key at construction", "value at construction"}});

	// Instantiate the Worker in the plugin and wrap it in a Worker wrapper here on the host.
	auto worker =
		plugin.load_adapter<feltpluginsystemdemohost::client::Worker>("fpdemo_Worker_suite", dict);

	// Do some work with the worker.

	try
	{
		worker.update_dict("first key from host");
	}
	catch (std::invalid_argument const & ex)
	{
		std::cout << "Invalid argument error from plugin caught in host: " << ex.what()
				  << std::endl;
	}

	for (auto [k, v] : *dict) std::cout << k << " = " << v << std::endl;

	std::cout << "Try again:" << std::endl;

	dict->insert({"plugin expects to exist", "value from host"});

	worker.update_dict("second key from host");

	for (auto [k, v] : *dict) std::cout << k << " = " << v << std::endl;
}
}  // namespace feltpluginsystemdemohost

int main()
{
	feltpluginsystemdemohost::execute();

	return 0;
}