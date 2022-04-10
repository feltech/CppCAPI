// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include <iostream>

#include <feltpluginsystem/plugin_definition.hpp>

#include <feltpluginsystem-demo-hello_plugin/interface.h>

#include "plugin_export.h"

namespace feltpluginsystemdemoplugin
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
using Plugin = feltplugin::PluginDefinition<
	// Service
	feltplugin::service::HandleMap<
		// Worker
		feltplugin::service::HandleTraits<
			fpdemo_Worker_h,
			Worker,
			feltplugin::service::HandleOwnershipTag::OwnedByClient>>>;
}  // namespace feltpluginsystemdemoplugin

extern "C"
{
	using feltpluginsystemdemoplugin::Plugin;
	using feltpluginsystemdemoplugin::Worker;

	// Plugin

	FELTPLUGINSYSTEM_DEMO_PLUGIN_EXPORT fpdemo_Worker_s fpdemo_Worker_suite()
	{
		using Decorator = Plugin::SuiteDecorator<fpdemo_Worker_h>;

		return {
			.create = &Plugin::HandleManager<fpdemo_Worker_h>::make,

			.release = &Plugin::HandleManager<fpdemo_Worker_h>::release,

			.work = Decorator::decorate(Decorator::mem_fn_ptr<&Worker::work>)};
	}
}