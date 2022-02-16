#include "receiver.hpp"

namespace feltplugindemohost::receiver
{

Plugin::Plugin(SuiteFactory suite_factory, feltplugin::SharedPtr<sender::StringDict> const & dict)
	: Base{suite_factory}
{
	auto hdict = sender::HandleFactory<fpdemo_StringDict_h>::create(dict);
	create(hdict);
}

void Plugin::update_dict(const sender::String & key)
{
	call(suite_.update_dict, sender::HandleFactory<fpdemo_String_h>::make(key));
}
}  // namespace feltplugindemohost::receiver
