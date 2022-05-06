// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include <iostream>

#include <cppcapi/plugin_definition.hpp>

#include <cppcapi-demo-hello_plugin/interface.h>

#include "plugin_export.h"

namespace cppcapidemoplugin
{

// The worker that will be instantiated then called by the host.
struct Worker
{
	void work()	 // NOLINT(readability-convert-member-functions-to-static)
	{
		std::cout << "... plugin service doing work: hello world from the plugin!" << std::endl;
	}
};

// Define C <-> C++ interface.
using Plugin = cppcapi::PluginDefinition<
	// Service
	cppcapi::service::HandleMap<
		// Worker
		cppcapi::service::HandleTraits<
			fpdemo_Worker_h,
			Worker,
			cppcapi::service::HandleOwnershipTag::OwnedByClient>>>;
}  // namespace cppcapidemoplugin

extern "C"
{
	using cppcapidemoplugin::Plugin;
	using cppcapidemoplugin::Worker;

	// Plugin

	CPPCAPI_DEMO_PLUGIN_EXPORT fpdemo_Worker_s fpdemo_Worker_suite()
	{
		using Decorator = Plugin::SuiteDecorator<fpdemo_Worker_h>;

		return {
			.create = &Plugin::HandleManager<fpdemo_Worker_h>::make,

			.release = &Plugin::HandleManager<fpdemo_Worker_h>::release,

			.work = Decorator::decorate(Decorator::mem_fn_ptr<&Worker::work>)};
	}
}