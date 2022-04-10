// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <feltpluginsystem/plugin_definition.hpp>
#include <feltpluginsystem/loader.hpp>

#include <feltpluginsystem-demo-hello_plugin/interface.h>

struct Worker;

// Define C <-> C++ interface.
using Plugin = feltplugin::PluginDefinition<
	// Client
	feltplugin::client::HandleMap<
		// Worker
		feltplugin::client::HandleTraits<fpdemo_Worker_h, fpdemo_Worker_s, Worker>>>;

struct Worker : Plugin::SuiteAdapter<fpdemo_Worker_h>
{
	using Base::SuiteAdapter;

	void work()
	{
		call(suite_.work);
	}
};

int main()
{
	// Calculate plugin path.
	char const * plugin_path_var = std::getenv("FELTPLUGINSYSTEM_PLUGIN_PATH");
	if (!plugin_path_var)
		throw std::invalid_argument{"FELTPLUGINSYSTEM_PLUGIN_PATH must be set"};
	std::filesystem::path plugin_path{plugin_path_var};

	plugin_path /= "libfeltpluginsystem-demo-hello_plugin-plugin.so";
	std::cout << "Loading plugin at " << plugin_path << std::endl;

	// Load the plugin DSO.
	feltplugin::Loader loader{plugin_path.c_str()};

	auto worker = loader.load_adapter<Worker>("fpdemo_Worker_suite");

	std::cout << "Host client telling plugin service to do work..." << std::endl;

	worker.work();

	return 0;
}