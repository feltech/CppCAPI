// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
#include "client.hpp"
#include "service.hpp"

namespace feltpluginsystemdemohost::client
{

Worker::Worker(SuiteFactory suite_factory, feltplugin::SharedPtr<service::StringDict> dict)
	: Base{suite_factory}
{
	auto hdict = HandleWrapper::Converter<fpdemo_StringDict_h>::create(std::move(dict));
	create(hdict);
}

void Worker::update_dict(service::StringView key)
{
	auto hkey = HandleWrapper::Converter<fpdemo_StringView_h>::create(key);
	call(suite_.update_dict, hkey);
}
}  // namespace feltpluginsystemdemohost::client
