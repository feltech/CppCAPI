#pragma once

#include "feltplugindemohostlib_export.h"
#include "handle_map.hpp"

namespace feltplugindemohost::client
{
struct FELTPLUGINDEMOHOSTLIB_EXPORT Worker : HandleAdapter<fpdemo_Worker_h>
{
	Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<owner::StringDict> dict);

	void update_dict(owner::StringView key);
};

}  // namespace feltplugindemohost::receiver
