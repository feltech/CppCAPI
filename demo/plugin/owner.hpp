#pragma once

#include <feltplugin/owner/handle_map.hpp>
#include <feltplugin/owner/handle_factory.hpp>

#include <feltplugindemo/interface.h>

#include "client.hpp"

namespace feltplugindemoplugin::owner
{
class Plugin;

// clang-format off
using HandleMap = feltplugin::owner::HandleMap<
	feltplugin::owner::HandleTraits<fpdemo_Plugin_h, feltplugin::UniquePtr<Plugin>, Plugin>
>;
// clang-format on

template <class Class>
using HandleFactory = feltplugin::owner::HandleFactory<Class, HandleMap>;


class Plugin
{
public:
	explicit Plugin(client::StringDict dict);
	~Plugin() = default;

	void update_dict(client::String key);

private:
	client::StringDict dict_;
};
}
