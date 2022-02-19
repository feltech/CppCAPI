#pragma once

#include <feltpluginsystem/service/handle_factory.hpp>
#include <feltpluginsystem/service/handle_map.hpp>

#include <feltpluginsystemdemo/interface.h>

#include "client.hpp"

namespace feltplugindemoplugin::service
{
class Worker
{
public:
	explicit Worker(client::StringDict dict);
	~Worker() = default;

	void update_dict(client::String const & key);

private:
	client::StringDict dict_;
};
}  // namespace feltplugindemoplugin::service
