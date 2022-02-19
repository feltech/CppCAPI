#pragma once

#include "feltplugindemohostlib_export.h"
#include "handle_wrapper.hpp"

namespace feltplugindemohost::client
{
struct FELTPLUGINDEMOHOSTLIB_EXPORT Worker : HandleWrapper::Adapter<fpdemo_Worker_h>
{
	Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<service::StringDict> dict);

	void update_dict(service::StringView key);
};

}  // namespace feltplugindemohost::client
