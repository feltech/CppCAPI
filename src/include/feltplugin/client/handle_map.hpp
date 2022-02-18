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
	using type = std::false_type;
};

struct fallback_class_t : std::false_type
{
	using type = std::false_type;
};

struct fallback_suite_factory_t : std::false_type
{
	using type = std::false_type;
};

template <class Traits, class... Rest>
struct HandleMap
{
	template <class HandleToLookup>
	using class_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template class_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template class_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using suite_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template suite_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template suite_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using suite_factory_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template suite_factory_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template suite_factory_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;

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
	using Class = typename Traits::Class;
	using Suite = typename Traits::Suite;
	static constexpr auto get_suite = Traits::suite_factory;

	template <class HandleToLookup>
	struct this_suite_from_handle_t : std::is_same<Handle, HandleToLookup>
	{
		using type = Suite;
	};

	template <class HandleToLookup>
	struct this_class_from_handle_t : std::is_same<Handle, HandleToLookup>
	{
		using type = Class;
	};

	template <class HandleToLookup>
	struct this_suite_factory_from_handle_t : std::is_same<Handle, HandleToLookup>
	{
		static constexpr auto type = get_suite;
	};

	template <class HandleToLookup>
	using class_from_handle_t =
		typename std::disjunction<this_class_from_handle_t<HandleToLookup>, fallback_class_t>;

	template <class HandleToLookup>
	using suite_from_handle_t =
		typename std::disjunction<this_suite_from_handle_t<HandleToLookup>, fallback_suite_t>;

	template <class HandleToLookup>
	using suite_factory_from_handle_t = typename std::
		disjunction<this_suite_factory_from_handle_t<HandleToLookup>, fallback_suite_factory_t>;

	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;

	template <class HandleToLookup>
	using suite_from_handle = typename suite_from_handle_t<HandleToLookup>::type;

	template <class HandleToLookup>
	static constexpr auto suite_factory_from_handle()
	{
		return suite_factory_from_handle_t<HandleToLookup>::type;
	};
};
}  // namespace feltplugin::client