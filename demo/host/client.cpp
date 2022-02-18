#include "client.hpp"

namespace feltplugindemohost::client
{

Worker::Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<owner::StringDict> dict)
	: Base{suite_factory}
{
	auto hdict = owner::HandleFactory<fpdemo_StringDict_h>::create(std::move(dict));
	create(hdict);
}

void Worker::update_dict(owner::StringView key)
{
	auto hkey = owner::HandleFactory<fpdemo_StringView_h>::create(key);
	call(suite_.update_dict, hkey);
}
}  // namespace feltplugindemohost::receiver
