// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the HandleMap of HandleTraits to be used by clients converting to/from opaque handles.
 */
#pragma once

#include <type_traits>

namespace cppcapi::client
{
/**
 * Client-specific traits for a particular opaque handle type.
 *
 * @tparam THandle Type of opaque handle.
 * @tparam TSuite Function pointer suite struct associated with handle.
 * @tparam TClass Adapter class responsible for wrapping handles of Handle type on the client.
 * @tparam Tsuite_factory Function returning a function pointer suite, if available at compile-time.
 */
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
	static constexpr auto type = nullptr;
};

/**
 * Utility to look up the traits of a given opaque handle.
 *
 * This specialisation is chosen if two or more HandleTraits are provided to the HandleMap.
 *
 * @tparam Rest Remaining opaque handles' traits.
 */
template <class... Rest>
struct HandleMap
{
	template <class HandleToLookup>
	using class_from_handle_t = typename std::disjunction<
		typename HandleMap<Rest>::template class_from_handle_t<HandleToLookup>...>;

	template <class HandleToLookup>
	using suite_from_handle_t = typename std::disjunction<
		typename HandleMap<Rest>::template suite_from_handle_t<HandleToLookup>...>;

	template <class HandleToLookup>
	using suite_factory_from_handle_t = typename std::disjunction<
		typename HandleMap<Rest>::template suite_factory_from_handle_t<HandleToLookup>...>;

	/**
	 * Find the adapter class associated with the given handle type.
	 *
	 * @tparam HandleToLookup Handle type to look up in traits list.
	 */
	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;

	/**
	 * Find the function pointer suite type associated with the given handle type.
	 *
	 * @tparam HandleToLookup Handle type to look up in traits list.
	 */
	template <class HandleToLookup>
	using suite_from_handle = typename suite_from_handle_t<HandleToLookup>::type;

	/**
	 * Find the function pointer suite factory function associated with the given handle type.
	 *
	 * Note that the factory function can be null, typically because the symbol is not available
	 * at compile-time - see HandleTraits.
	 *
	 * @tparam HandleToLookup Handle type to look up in traits list.
	 * @return Function pointer to suite factory.
	 */
	template <class HandleToLookup>
	static constexpr auto suite_factory_from_handle()
	{
		return suite_factory_from_handle_t<HandleToLookup>::type;
	}
};

/**
 * Utility to look up the traits of a given opaque handle.
 *
 * This specialisation is chosen if a single HandleTrait is provided to the HandleMap.
 *
 * @tparam Traits Opaque handle traits class.
 */
template <class Traits>
struct HandleMap<Traits>
{
	/// Hoist handle type from traits.
	using Handle = typename Traits::Handle;
	/// Hoist adapter class from traits.
	using Class = typename Traits::Class;
	/// Hoist function pointer suite class from traits.
	using Suite = typename Traits::Suite;
	/// Hoist function pointer suite factory from traits.
	static constexpr auto suite_factory = Traits::suite_factory;

private:
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
		static constexpr auto type = suite_factory;
	};

public:
	template <class HandleToLookup>
	using class_from_handle_t =
		typename std::disjunction<this_class_from_handle_t<HandleToLookup>, fallback_class_t>;

	template <class HandleToLookup>
	using suite_from_handle_t =
		typename std::disjunction<this_suite_from_handle_t<HandleToLookup>, fallback_suite_t>;

	template <class HandleToLookup>
	using suite_factory_from_handle_t = typename std::
		disjunction<this_suite_factory_from_handle_t<HandleToLookup>, fallback_suite_factory_t>;

	/**
	 * Get the adapter class associated with our Handle if HandleToLookup matches, otherwise
	 * `std::false_type`.
	 *
	 * @tparam HandleToLookup Handle type to compare with ours.
	 */
	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;

	/**
	 * Get the function pointer suite type associated with our Handle if HandleToLookup matches,
	 * otherwise `std::false_type`.
	 *
	 * @tparam HandleToLookup Handle type to compare with ours.
	 */
	template <class HandleToLookup>
	using suite_from_handle = typename suite_from_handle_t<HandleToLookup>::type;

	/**
	 * Get the function pointer suite factory function associated with our Handle if HandleToLookup
	 * matches, otherwise `nullptr`.
	 *
	 * Note that the factory function can be explicitly set to `nullptr`, typically because the
	 * symbol is not available at compile-time - @see HandleTraits.
	 *
	 * @tparam HandleToLookup Handle type to compare with ours.
	 * @return Function pointer to suite factory.
	 */
	template <class HandleToLookup>
	static constexpr auto suite_factory_from_handle()
	{
		return suite_factory_from_handle_t<HandleToLookup>::type;
	}
};

/**
 * Utility to look up the traits of a given opaque handle.
 *
 * This specialisation is chosen if a no HandleTraits are provided to the HandleMap.
 */
template <>
struct HandleMap<>
{
	/**
	 * Give the same fallback type no matter what handle type is given.
	 *
	 * @tparam HandleToLookup Handle type to look up.
	 */
	template <class HandleToLookup>
	using class_from_handle = fallback_class_t::type;

	/**
	 * Give the same fallback type no matter what handle type is given.
	 *
	 * @tparam HandleToLookup Handle type to look up.
	 */
	template <class HandleToLookup>
	using suite_from_handle = fallback_suite_t::type;

	/**
	 * Give the same fallback type no matter what handle type is given.
	 *
	 * @tparam HandleToLookup Handle type to look up.
	 */
	template <class HandleToLookup>
	static constexpr auto suite_factory_from_handle()
	{
		return fallback_suite_factory_t::type;
	}
};
}  // namespace cppcapi::client