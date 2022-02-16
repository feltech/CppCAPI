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
	// Plugin
	feltplugin::client::HandleTraits<
		fpdemo_Plugin_h , fpdemo_Plugin_s, nullptr, struct Plugin>
>;
// clang-format on

template <class THandle>
using HandleAdapter = feltplugin::client::HandleAdapter<THandle, HandleMap>;

struct FELTPLUGINDEMOHOSTLIB_EXPORT Plugin : HandleAdapter<fpdemo_Plugin_h>
{
	Plugin(SuiteFactory suite_factory, feltplugin::SharedPtr<owner::StringDict> const & dict);

	void update_dict(const owner::String & key);
};

}  // namespace feltplugindemohost::receiver
