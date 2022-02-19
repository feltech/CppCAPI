#pragma once

namespace feltplugin::service
{

enum class HandlePtrTag
{
	Shared,
	OwnedByClient,
	OwnedByService,
	Unrecognized
};

template <class THandle, class TClass, HandlePtrTag Tptr_type_tag>
struct HandleTraits
{
	using Handle = THandle;
	using Class = TClass;
	static constexpr HandlePtrTag ptr_type_tag = Tptr_type_tag;
};

/**
 * Fallback default: assume if no match then must be C-native type, so pass-through.
 * TODO: Use for auto-converting function arguments.
 */
template <class HandleToLookup>
struct fallback_class_t : std::false_type
{
	using type = HandleToLookup;
};

template <class HandleToLookup>
struct fallback_ptr_tag_t : std::false_type
{
	static constexpr auto type = HandlePtrTag::Unrecognized;
};

template <class Traits, class... Rest>
struct HandleMap
{
	template <class HandleToLookup>
	using ptr_tag_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template ptr_tag_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template ptr_tag_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	using class_from_handle_t = typename std::disjunction<
		typename HandleMap<Traits>::template class_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template class_from_handle_t<HandleToLookup>>;

	template <class HandleToLookup>
	static constexpr auto ptr_tag_from_handle()
	{
		return ptr_tag_from_handle_t<HandleToLookup>::type;
	};
	template <class HandleToLookup>
	using class_from_handle = typename class_from_handle_t<HandleToLookup>::type;
};

template <class Traits>
struct HandleMap<Traits>
{
	using Handle = typename Traits::Handle;
	using Class = typename Traits::Class;
	static constexpr HandlePtrTag Ptr = Traits::ptr_type_tag;

	template <class Other>
	struct this_ptr_tag_from_handle_t : std::is_same<Handle, Other>
	{
		static constexpr auto type = Ptr;
	};

	template <class Other>
	struct this_class_from_handle_t : std::is_same<Handle, Other>
	{
		using type = Class;
	};

	template <class HandleToLookup>
	using ptr_tag_from_handle_t = typename std::
		disjunction<this_ptr_tag_from_handle_t<HandleToLookup>, fallback_ptr_tag_t<HandleToLookup>>;

	template <class HandleToLookup>
	using class_from_handle_t = typename std::
		disjunction<this_class_from_handle_t<HandleToLookup>, fallback_class_t<HandleToLookup>>;

	template <class HandleToLookup>
	static constexpr auto ptr_tag_from_handle()
	{
		return ptr_tag_from_handle_t<HandleToLookup>::type;
	};

	template <class Arg>
	using class_from_handle = typename class_from_handle_t<Arg>::type;
};
}  // namespace feltplugin::service