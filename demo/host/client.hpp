#pragma once

#include <feltplugin/client/handle_adaptor.hpp>
#include <feltplugin/client/handle_map.hpp>

#include <feltplugindemo/interface.h>

#include "feltplugindemohostlib_export.h"
#include "owner.hpp"

namespace feltplugindemohost::client
{

// clang-format off
using HandleMap = feltplugin::client::HandleMap<
	// Worker
	feltplugin::client::HandleTraits<
		fpdemo_Worker_h , fpdemo_Worker_s, struct Plugin>
>;
// clang-format on

template <class THandle>
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap>;

struct FELTPLUGINDEMOHOSTLIB_EXPORT Worker : HandleAdapter<fpdemo_Worker_h>
{
	Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<owner::StringDict> dict);

	void update_dict(const owner::String & key);
};

}  // namespace feltplugindemohost::receiver
