// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the HandleMap of HandleTraits to be used by services converting to/from opaque handles.
 */
#pragma once

namespace cppcapi::service
{

/// Tag enum to indicate ownership of a handle.
enum class HandleOwnershipTag
{
	Shared,
	OwnedByClient,
	OwnedByService,
	Unrecognized  // For internal use only!
};

/**
 * Service-specific traits for a particular opaque handle type.
 *
 * @tparam THandle Type of opaque handle.
 * @tparam TClass Native class associated with handle.
 * @tparam Townership_tag Ownership model tag.
 */
template <class THandle, class TClass, HandleOwnershipTag Townership_tag>
struct HandleTraits
{
	using Handle = THandle;
	using Class = TClass;
	static constexpr HandleOwnershipTag ownership_tag = Townership_tag;
};

/**
 * Fallback default to assume a C-native type, so pass-through.
 *
 * @tparam HandleToLookup Handle type to pass through as class type.
 */
template <class HandleToLookup>
struct fallback_class_t : std::false_type
{
	using type = HandleToLookup;
};

/**
 * Fallback default to an unknown handle hence unrecognized ownership tag.
 *
 * @tparam HandleToLookup Ignored.
 */
template <class HandleToLookup>
struct fallback_ownership_tag_t : std::false_type
{
	static constexpr auto type = HandleOwnershipTag::Unrecognized;
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
	using ownership_tag_from_handle_t = typename std::disjunction<
		typename HandleMap<Rest>::template ownership_tag_from_handle_t<HandleToLookup>...>;

	template <class HandleToLookup>
	using class_from_handle_t = typename std::disjunction<
		typename HandleMap<Rest>::template class_from_handle_t<HandleToLookup>...>;

	/**
	 * Find the ownership model for the given handle type.
	 *
	 * @tparam HandleToLookup Handle type to look up in traits list.
	 */
	template <class HandleToLookup>
	static constexpr auto ownership_tag_from_handle()
	{
		return ownership_tag_from_handle_t<HandleToLookup>::type;
	}

	/**
	 * Find the native class associated with the given handle type.
	 *
	 * @tparam HandleToLookup Handle type to look up in traits list.
	 */
	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;
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
	/// Hoist native class from traits.
	using Class = typename Traits::Class;
	/// Hoist ownership tag from traits.
	static constexpr HandleOwnershipTag ownership_tag = Traits::ownership_tag;

private:
	template <class Other>
	struct this_ownership_tag_from_handle_t : std::is_same<Handle, Other>
	{
		static constexpr auto type = ownership_tag;
	};

	template <class Other>
	struct this_class_from_handle_t : std::is_same<Handle, Other>
	{
		using type = Class;
	};

public:
	template <class HandleToLookup>
	using ownership_tag_from_handle_t = typename std::disjunction<
		this_ownership_tag_from_handle_t<HandleToLookup>,
		fallback_ownership_tag_t<HandleToLookup>>;

	template <class HandleToLookup>
	using class_from_handle_t = typename std::
		disjunction<this_class_from_handle_t<HandleToLookup>, fallback_class_t<HandleToLookup>>;

	/**
	 * Get the ownership tag associated with our Handle if HandleToLookup matches, otherwise
	 * HandleOwnershipTag::Unrecognized.
	 *
	 * @tparam HandleToLookup Handle type to compare with ours.
	 * @return Handle ownership tag.
	 */
	template <class HandleToLookup>
	static constexpr auto ownership_tag_from_handle()
	{
		return ownership_tag_from_handle_t<HandleToLookup>::type;
	}
	/**
	 * Get the native class associated with our Handle if HandleToLookup matches, otherwise
	 * pass-through the HandleToLookup.
	 *
	 * @tparam HandleToLookup Handle type to compare with ours.
	 */
	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;
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
	 * Give HandleOwnershipTag::Unrecognized regardless of HandleToLookup.
	 *
	 * @tparam HandleToLookup Ignored.
	 * @return Always HandleOwnershipTag::Unrecognized.
	 **/
	template <class HandleToLookup>
	static constexpr auto ownership_tag_from_handle()
	{
		return fallback_ownership_tag_t<HandleToLookup>::type;
	}

	/**
	 * Always pass-through HandleToLookup.
	 *
	 * @tparam HandleToLookup Handle type to pass through.
	 */
	template <class HandleToLookup>
	using class_from_handle = typename fallback_class_t<HandleToLookup>::type;
};
}  // namespace cppcapi::service