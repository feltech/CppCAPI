// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#pragma once

#include "client.hpp"

namespace feltpluginsystemdemoplugin::service
{
class Worker
{
public:
	explicit Worker(client::StringDict service_dict);
	~Worker() = default;

	void update_dict(client::String const & key);

private:
	client::StringDict service_dict_;
	client::StringDict client_dict_;
};
}  // namespace feltpluginsystemdemoplugin::service
