#pragma once

#include <type_traits>

#include "feltplugin/pointers.hpp"

namespace feltplugin::client
{

template <class THandle, class TSuite, class TClass, TSuite (*Tsuite_factory)() = nullptr>
struct HandleTraits
{
	using Handle = THandle;
	using Suite = TSuite;
	using Class = TClass;
	static constexpr auto suite_factory = Tsuite_factory;
};


struct fallback_suite_t : std::false_type
{
	using type = struct handle_not_found_in_client_HandleMap;
};

struct fallback_suite_factory_t : std::false_type
{
	using type = struct handle_not_found_in_client_HandleMap;
};

template <class Traits, class... Rest>
struct HandleMap
{
	template <class HandleToLookup>
	using suite_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template suite_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template suite_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using suite_factory_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template suite_factory_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template suite_factory_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using suite_from_handle = typename suite_from_handle_t<HandleToLookup>::type;

	template <class HandleToLookup>
	static constexpr auto suite_factory_from_handle()
	{
		return suite_factory_from_handle_t<HandleToLookup>::type;
	};
};

template <class Traits>
struct HandleMap<Traits>
{
	using Handle = typename Traits::Handle;
	using Suite = typename Traits::Suite;
	static constexpr auto get_suite = Traits::suite_factory;

	template <class HandleToLookup>
	struct this_suite_from_handle_t : std::is_same<Handle, HandleToLookup>
	{
		using type = Suite;
	};

	template <class HandleToLookup>
	struct this_suite_factory_from_handle_t : std::is_same<Handle, HandleToLookup>
	{
		static constexpr auto type = get_suite;
	};

	template <class HandleToLookup>
	using suite_from_handle_t =
		typename std::disjunction<this_suite_from_handle_t<HandleToLookup>, fallback_suite_t>;

	template <class HandleToLookup>
	using suite_factory_from_handle_t = typename std::disjunction<
		this_suite_factory_from_handle_t<HandleToLookup>,
		fallback_suite_factory_t>;

	template <class HandleToLookup>
	using suite_from_handle = typename suite_from_handle_t<HandleToLookup>::type;

	template <class HandleToLookup>
	static constexpr auto suite_factory_from_handle()
	{
		return suite_factory_from_handle_t<HandleToLookup>::type;
	};
};
}  // namespace feltplugin::client