// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the HandleWrapper top-level convenience alias aggregation.
 */
#pragma once

#include "client/handle_adaptor.hpp"
#include "client/handle_map.hpp"
#include "error_map.hpp"
#include "service/handle_factory.hpp"
#include "service/handle_map.hpp"

namespace feltplugin
{
/**
 * Convenience alias aggregation of service::HandleConverter, service::SuiteDecorator and
 * client::HandleAdapter and their common template parameters.
 *
 * @tparam TServiceHandleMap service::HandleMap mapping handles to native classes.
 * @tparam TClientHandleMap client::HandleMap mapping handles to adapter classes.
 * @tparam TErrorMap ErrorMap mapping exceptions to error codes.
 */
template <class TServiceHandleMap, class TClientHandleMap, class TErrorMap = ErrorMap<>>
struct HandleWrapper
{
	template <class THandle>
	using Decorator =
		service::SuiteDecorator<THandle, TServiceHandleMap, TClientHandleMap, TErrorMap>;

	template <class THandle>
	using Converter =
		service::HandleConverter<THandle, TServiceHandleMap, TClientHandleMap, TErrorMap>;

	template <class THandle>
	using Adapter = client::SuiteAdapter<THandle, TClientHandleMap, TErrorMap>;
};
}  // namespace feltplugin