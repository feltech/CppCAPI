// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once

#include "plugin_definition.hpp"
#include "host_export.h"

namespace feltpluginsystemdemohost::client
{
struct FELTPLUGINSYSTEM_DEMO_HOST_EXPORT Worker : Plugin::SuiteAdapter<fpdemo_Worker_h>
{
	Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<service::StringDict> dict);

	void update_dict(service::StringView key);
};

}  // namespace feltpluginsystemdemohost::client
