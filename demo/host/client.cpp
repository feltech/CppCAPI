#include "client.hpp"

namespace feltplugindemohost::client
{

Worker::Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<owner::StringDict> const & dict)
	: Base{suite_factory}
{
	auto hdict = owner::HandleFactory<fpdemo_StringDict_h>::create(dict);
	create(hdict);
}

void Worker::update_dict(const owner::String & key)
{
	auto hkey = owner::HandleFactory<fpdemo_String_h>::make(key);
	call(suite_.update_dict, hkey);
}
}  // namespace feltplugindemohost::receiver
