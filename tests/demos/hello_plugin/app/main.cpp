#include <iostream>

#include <feltpluginsystem/plugin_factory.hpp>
#include <feltpluginsystem/handle_wrapper.hpp>

#include <feltpluginsystem-demo-hello_plugin/interface.h>

struct Worker;

// Define C <-> C++ interface.
using HandleWrapper = feltplugin::HandleWrapper<
	// Service - nothing offered.
	feltplugin::service::HandleMap<>,

	// Client
	feltplugin::client::HandleMap<feltplugin::client::HandleMap<
		// Worker
		feltplugin::client::HandleTraits<fpdemo_Worker_h, fpdemo_Worker_s, Worker>>>>;


struct Worker : HandleWrapper::Adapter<fpdemo_Worker_h>
{
	using Base::HandleAdapter;

	void work()
	{
		call(suite_.work);
	}
};

int main()
{
	feltplugin::service::Plugin plugin{FELTPLUGINSYSTEM_PLUGIN_PATH
									   "libfeltpluginsystem-demo-hello_plugin-plugin.so"};

	auto worker = plugin.load_adapter<Worker>("fpdemo_Worker_suite");

	std::cout << "Host client telling plugin service to do work..." << std::endl;

	worker.work();

	return 0;
}