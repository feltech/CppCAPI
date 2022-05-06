// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once

#include "host_export.h"
#include "plugin_definition.hpp"

namespace cppcapidemohost::client
{
struct CPPCAPI_DEMO_HOST_EXPORT Worker : Plugin::SuiteAdapter<cppcapidemo_Worker_h>
{
	Worker(SuiteFactory suite_factory, cppcapi::SharedPtr<service::StringDict> dict);

	void update_dict(service::StringView key);
};

}  // namespace cppcapidemohost::client
