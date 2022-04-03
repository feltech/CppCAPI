// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include <iostream>

#include <feltpluginsystem/handle_wrapper.hpp>

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
using HandleWrapper = feltplugin::HandleWrapper<
	// Service
	feltplugin::service::HandleMap<
		// Worker
		feltplugin::service::HandleTraits<
			fpdemo_Worker_h,
			Worker,
			feltplugin::service::HandleOwnershipTag::OwnedByClient>>,

	// Client - nothing offered.
	feltplugin::client::HandleMap<>>;

}  // namespace feltpluginsystemdemoplugin

extern "C"
{
	using feltpluginsystemdemoplugin::HandleWrapper;
	using feltpluginsystemdemoplugin::Worker;

	// Plugin

	FELTPLUGINSYSTEM_DEMO_PLUGIN_EXPORT fpdemo_Worker_s fpdemo_Worker_suite()
	{
		return {
			.create = &HandleWrapper::Converter<fpdemo_Worker_h>::make,

			.release = &HandleWrapper::Converter<fpdemo_Worker_h>::release,

			.work = [](fpdemo_Worker_h handle)
			{
				return HandleWrapper::Factory<fpdemo_Worker_h>::mem_fn(
					[](Worker & self) { self.work(); }, handle);
			}};
	}
}