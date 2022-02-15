#pragma once

#include <type_traits>

#include "feltplugin/pointers.hpp"

namespace feltplugin::receiver
{

template <class THandle, class TSuite, TSuite (*Tsuite_factory)(), class TClass>
struct HandleTraits
{
	using Handle = THandle;
	using Suite = TSuite;
	using Class = TClass;
	static constexpr auto suite_factory = Tsuite_factory;
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
	static constexpr auto suite_factory_from_handle() {
		return suite_factory_from_handle_t<HandleToLookup>::type;
	};
};

template <class Traits>
struct HandleMap<Traits>
{
	using Handle =  typename Traits::Handle;
	using Suite =  typename Traits::Suite;
	static constexpr auto get_suite = Traits::suite_factory;

	template <class Other>
	struct suite_from_handle_t : std::is_same<Handle, Other>
	{
		using type = Suite;
	};

	template <class Other>
	struct suite_factory_from_handle_t : std::is_same<Handle, Other>
	{
		static constexpr auto type = get_suite;
	};

	template <class Arg>
	using suite_from_handle = typename suite_from_handle_t<Arg>::type;

	template <class HandleToLookup>
	static constexpr auto suite_factory_from_handle() {
		return suite_factory_from_handle_t<HandleToLookup>::type;
	};
};
}