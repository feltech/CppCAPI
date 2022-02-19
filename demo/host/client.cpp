#include "client.hpp"
#include "service.hpp"

namespace feltplugindemohost::client
{

Worker::Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<service::StringDict> dict)
	: Base{suite_factory}
{
	auto hdict = HandleWrapper::Factory<fpdemo_StringDict_h>::create(std::move(dict));
	create(hdict);
}

void Worker::update_dict(service::StringView key)
{
	auto hkey = HandleWrapper::Factory<fpdemo_StringView_h>::create(key);
	call(suite_.update_dict, hkey);
}
}  // namespace feltplugindemohost::client
