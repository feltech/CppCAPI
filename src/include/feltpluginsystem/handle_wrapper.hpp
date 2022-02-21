#pragma once

#include "client/handle_adaptor.hpp"
#include "client/handle_map.hpp"
#include "error_map.hpp"
#include "service/handle_factory.hpp"
#include "service/handle_map.hpp"

namespace feltplugin
{

template <class TServiceHandleMap, class TClientHandleMap, class TErrorMap = ErrorMap<>>
struct HandleWrapper
{
	template <class Class>
	using Factory = service::HandleFactory<Class, TServiceHandleMap, TClientHandleMap, TErrorMap>;

	template <class THandle>
	using Adapter = client::HandleAdapter<THandle, TClientHandleMap, TErrorMap>;
};
}  // namespace feltplugin