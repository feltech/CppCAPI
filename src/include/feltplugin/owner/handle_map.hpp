#pragma once

#include <type_traits>

#include "feltplugin/pointers.hpp"

namespace feltplugin::owner
{
template <class THandle, class TPtr, class TClass>
struct HandleTraits
{
	using Handle = THandle;
	using Ptr = TPtr;
	using Class = TClass;
};

/**
 * Fallback default: assume if no match then must be C-native type, so pass-through.
 * TODO: Use for auto-converting function arguments.
 */
template <class HandleToLookup>
struct assume_c_native_class_t : std::false_type
{
	using type = HandleToLookup;
};

template <class HandleToLookup>
struct assume_c_native_ptr_t : std::false_type
{
	using type = HandleToLookup *;
};

template <class Traits, class... Rest>
struct HandleMap
{
	template <class HandleToLookup>
	using ptr_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template ptr_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template ptr_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using class_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template class_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template class_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using ptr_from_handle = typename ptr_from_handle_t<HandleToLookup>::type;

	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;
};

template <class Traits>
struct HandleMap<Traits>
{
	using Handle = typename Traits::Handle;
	using Ptr = typename Traits::Ptr;
	using Class = typename Traits::Class;

	template <class Other>
	struct this_ptr_from_handle_t : std::is_same<Handle, Other>
	{
		using type = Ptr;
	};

	template <class Other>
	struct this_class_from_handle_t : std::is_same<Handle, Other>
	{
		using type = Class;
	};

	template <class HandleToLookup>
	using ptr_from_handle_t = typename std::
		disjunction<this_ptr_from_handle_t<HandleToLookup>, assume_c_native_ptr_t<HandleToLookup>>;

	template <class HandleToLookup>
	using class_from_handle_t = typename std::disjunction<
		this_class_from_handle_t<HandleToLookup>,
		assume_c_native_class_t<HandleToLookup>>;

	template <class Arg>
	using ptr_from_handle = typename ptr_from_handle_t<Arg>::type;

	template <class Arg>
	using class_from_handle = typename class_from_handle_t<Arg>::type;
};
}  // namespace feltplugin::owner