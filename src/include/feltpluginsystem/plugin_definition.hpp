// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the HandleWrapper top-level convenience alias aggregation.
 */
#pragma once

#include "client/handle_map.hpp"
#include "client/suite_adaptor.hpp"
#include "error_map.hpp"
#include "service/handle_map.hpp"
#include "service/suite_decorator.hpp"

namespace feltplugin
{

namespace detail
{

template <typename... Args>
struct plugin_definition_args_t;

template <>
struct plugin_definition_args_t<>
{
	using service_handle_map_t = service::HandleMap<>;
	using client_handle_map_t = client::HandleMap<>;
	using error_map_t = ErrorMap<>;
};

template <typename... ArgArgs, class... OtherArgs>
struct plugin_definition_args_t<service::HandleMap<ArgArgs...>, OtherArgs...>
	: plugin_definition_args_t<OtherArgs...>
{
	using service_handle_map_t = service::HandleMap<ArgArgs...>;
};

template <typename... ArgArgs, class... OtherArgs>
struct plugin_definition_args_t<client::HandleMap<ArgArgs...>, OtherArgs...>
	: plugin_definition_args_t<OtherArgs...>
{
	using client_handle_map_t = client::HandleMap<ArgArgs...>;
};

template <typename... ArgArgs, class... OtherArgs>
struct plugin_definition_args_t<ErrorMap<ArgArgs...>, OtherArgs...>
	: plugin_definition_args_t<OtherArgs...>
{
	using error_map_t = ErrorMap<ArgArgs...>;
};

}  // namespace detail

/**
 * Defines, via template parameters, the set of C++ types and their associated C API handles and
 * converters between them.
 *
 * Essentially a convenience alias aggregation of service::SuiteDecorator, service::HandleManager
 * and client::SuiteAdapter and their common template parameters.
 *
 * @tparam Args service::HandleMap mapping handles to native classes, client::HandleMap mapping
 * handles to adapter classes, and ErrorMap mapping exceptions to error codes, in any order. If any
 * of those are missing, then an empty fallback is assumed.
 */
template <class... Args>
class PluginDefinition
{
	using arg_parse_t = detail::plugin_definition_args_t<Args...>;
	using ServiceHandleMap = typename arg_parse_t::service_handle_map_t;
	using ClientHandleMap = typename arg_parse_t::client_handle_map_t;
	using ErrorMap = typename arg_parse_t::error_map_t;

public:
	template <class Handle>
	using SuiteDecorator =
		service::SuiteDecorator<Handle, ServiceHandleMap, ClientHandleMap, ErrorMap>;

	template <class Handle>
	using HandleManager =
		service::HandleManager<Handle, ServiceHandleMap, ClientHandleMap, ErrorMap>;

	template <class Handle>
	using SuiteAdapter = client::SuiteAdapter<Handle, ClientHandleMap, ErrorMap>;
};
}  // namespace feltplugin