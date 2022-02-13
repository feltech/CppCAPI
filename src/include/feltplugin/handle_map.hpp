#pragma once

#include <type_traits>

#include "pointers.hpp"

namespace feltplugin
{

template <class Handle, class Ptr, class... Rest>
struct HandleMap
{
	template <class ClassToLookup>
	using handle_from_ptr_t = typename std::disjunction<
		typename HandleMap<Handle, Ptr>::template handle_from_ptr_t<ClassToLookup>,
		typename HandleMap<Rest...>::template handle_from_ptr_t<ClassToLookup>>;

	template <class HandleToLookup>
	using ptr_from_handle_t = typename std::disjunction<
		typename HandleMap<Handle, Ptr>::template ptr_from_handle_t<HandleToLookup>,
		typename HandleMap<Rest...>::template ptr_from_handle_t<HandleToLookup>>;

	template <class PtrToLookup>
	using ptr_from_class_t = typename std::disjunction<
		typename HandleMap<Handle, Ptr>::template ptr_from_class_t<PtrToLookup>,
		typename HandleMap<Rest...>::template ptr_from_class_t<PtrToLookup>>;

	template <class PtrToLookup>
	using handle_from_ptr = typename handle_from_ptr_t<PtrToLookup>::type;

	template <class HandleToLookup>
	using ptr_from_handle = typename ptr_from_handle_t<HandleToLookup>::type;

	template <class PtrToLookup>
	using ptr_from_class = typename ptr_from_class_t<PtrToLookup>::type;
};

template <class Handle, class Ptr>
struct HandleMap<Handle, Ptr>
{
	template <class Other>
	struct ptr_from_handle_t : std::is_same<Handle, Other>
	{
		using type = Ptr;
	};

	template <class Other>
	struct handle_from_ptr_t : std::is_same<Ptr, Other>
	{
		using type = Handle;
	};

	template <class Other>
	struct ptr_from_class_t : std::disjunction<
								  std::is_same<Ptr, RawPtr<Other>>,
								  std::is_same<Ptr, UniquePtr<Other>>,
								  std::is_same<Ptr, SharedPtr<Other>>>
	{
		using type = Ptr;
	};

	template <class Arg>
	using handle_from_ptr = typename handle_from_ptr_t<Arg>::type;

	template <class Arg>
	using ptr_from_handle = typename ptr_from_handle_t<Arg>::type;

	template <class Arg>
	using ptr_from_class = typename ptr_from_class_t<Arg>::type;
};
}