#pragma once

#include <feltplugin/service/handle_factory.hpp>
#include <feltplugin/service/handle_map.hpp>

#include <feltplugindemo/interface.h>

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
