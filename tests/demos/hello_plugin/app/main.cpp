// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <cppcapi/loader.hpp>
#include <cppcapi/plugin_definition.hpp>

#include <cppcapi-demo-hello_plugin/interface.h>

struct Worker;

// Define C <-> C++ interface.
using Plugin = cppcapi::PluginDefinition<
	// Client
	cppcapi::client::HandleMap<
		// Worker
		cppcapi::client::HandleTraits<cppcapidemo_Worker_h, cppcapidemo_Worker_s, Worker>>>;

struct Worker : Plugin::SuiteAdapter<cppcapidemo_Worker_h>
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
	char const * plugin_path_var = std::getenv("CPPCAPI_PLUGIN_PATH");
	if (!plugin_path_var)
		throw std::invalid_argument{"CPPCAPI_PLUGIN_PATH must be set"};
	std::filesystem::path plugin_path{plugin_path_var};

	plugin_path /= "libcppcapi-demo-hello_plugin-plugin.so";
	std::cout << "Loading plugin at " << plugin_path << std::endl;

	// Load the plugin DSO.
	cppcapi::Loader loader{plugin_path.c_str()};

	auto worker = loader.load_adapter<Worker>("cppcapidemo_Worker_suite");

	std::cout << "Host client telling plugin service to do work..." << std::endl;

	worker.work();

	return 0;
}