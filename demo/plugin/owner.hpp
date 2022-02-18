#pragma once

#include <feltplugin/owner/handle_factory.hpp>
#include <feltplugin/owner/handle_map.hpp>

#include <feltplugindemo/interface.h>

#include "client.hpp"

namespace feltplugindemoplugin::owner
{
class Worker
{
public:
	explicit Worker(client::StringDict dict);
	~Worker() = default;

	void update_dict(client::String const& key);

private:
	client::StringDict dict_;
};
}  // namespace feltplugindemoplugin::owner
