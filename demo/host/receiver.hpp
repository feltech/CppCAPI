#pragma once

#include <feltplugin/receiver/handle_adaptor.hpp>
#include <feltplugin/receiver/handle_map.hpp>

#include <feltplugindemo/interface.h>

#include "feltplugindemohostlib_export.h"
#include "sender.hpp"

namespace feltplugindemohost::receiver
{

// clang-format off
using HandleMap = feltplugin::receiver::HandleMap<
	// Plugin
	feltplugin::receiver::HandleTraits<
		fpdemo_Plugin_h , fpdemo_Plugin_s, nullptr, struct Plugin>
>;
// clang-format on

template <class THandle>
using HandleAdapter = feltplugin::receiver::HandleAdapter<THandle, HandleMap>;

struct FELTPLUGINDEMOHOSTLIB_EXPORT Plugin : HandleAdapter<fpdemo_Plugin_h>
{
	Plugin(SuiteFactory suite_factory, feltplugin::SharedPtr<sender::StringDict> const & dict);

	void update_dict(const sender::String & key);
};

}  // namespace feltplugindemohost::receiver
