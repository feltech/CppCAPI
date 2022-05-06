// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once

#include "plugin_definition.hpp"
#include "host_export.h"

namespace cppcapidemohost::client
{
struct CPPCAPI_DEMO_HOST_EXPORT Worker : Plugin::SuiteAdapter<fpdemo_Worker_h>
{
	Worker(SuiteFactory suite_factory, cppcapi::SharedPtr<service::StringDict> dict);

	void update_dict(service::StringView key);
};

}  // namespace cppcapidemohost::client
