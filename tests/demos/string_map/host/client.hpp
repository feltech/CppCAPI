// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once

#include "handle_wrapper.hpp"
#include "host_export.h"

namespace feltpluginsystemdemohost::client
{
struct FELTPLUGINSYSTEM_DEMO_HOST_EXPORT Worker : HandleWrapper::Adapter<fpdemo_Worker_h>
{
	Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<service::StringDict> dict);

	void update_dict(service::StringView key);
};

}  // namespace feltpluginsystemdemohost::client
