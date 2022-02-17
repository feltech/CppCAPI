#pragma once

#include <feltplugin/owner/handle_factory.hpp>
#include <feltplugin/owner/handle_map.hpp>

#include <feltplugindemo/interface.h>

#include "client.hpp"

namespace feltplugindemoplugin::owner
{
using feltplugin::owner::HandlePtrTag;
class Worker;

// clang-format off
using HandleMap = feltplugin::owner::HandleMap<
	feltplugin::owner::HandleTraits<fpdemo_Worker_h, Worker, HandlePtrTag::OwnedByClient>
>;
// clang-format on

template <class Class>
using HandleFactory = feltplugin::owner::HandleFactory<Class, HandleMap>;

class Worker
{
public:
	explicit Worker(client::StringDict dict);
	~Worker() = default;

	void update_dict(client::String key);

private:
	client::StringDict dict_;
};
}  // namespace feltplugindemoplugin::owner
