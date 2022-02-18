#pragma once

#include <cstring>

#include "../errors.h"
#include "../pointers.hpp"
#include "handle_map.hpp"

namespace feltplugin::owner
{
namespace
{
template <class...>
struct always_false_t : std::false_type
{
};

template <typename Fn>
fp_ErrorCode wrap_exception(fp_ErrorMessage err, Fn && fn)
{
	try
	{
		fn();
	}
	catch (std::exception & ex)
	{
		strncpy(err, ex.what(), sizeof(fp_ErrorMessage));
		return fp_error;
	}
	return fp_ok;
}
}  // namespace

template <class THandle, class THandleMap>
struct HandleFactory
{
	using Handle = THandle;
	using Class = typename THandleMap::template class_from_handle<Handle>;

	static constexpr HandlePtrTag ptr_type_tag = THandleMap::template ptr_tag_from_handle<Handle>();

	using PtrRef =
		std::conditional_t<ptr_type_tag == HandlePtrTag::Shared, SharedPtr<Class> &, Class *>;

	template <typename... Args>
	static fp_ErrorCode make(char * err, Handle * out, Args... args)
	{
		return wrap_exception(err, [&out, &args...] { *out = make(std::forward<Args>(args)...); });
	}

	template <typename... Args>
	static Handle make(Args &&... args)
	{
		static_assert(
			ptr_type_tag != HandlePtrTag::OwnedByOwner,
			"Cannot make a handle to a new instance for non-shared non-transferred types");

		if constexpr (ptr_type_tag == HandlePtrTag::Shared)
		{
			return create(std::make_shared<Class>(std::forward<Args>(args)...));
		}
		else if constexpr (ptr_type_tag == HandlePtrTag::OwnedByClient)
		{
			return reinterpret_cast<Handle>(new Class{std::forward<Args>(args)...});
		}
		// C-native type.
		else if constexpr (ptr_type_tag == HandlePtrTag::Temporary)
		{
			return Class{std::forward<Args>(args)...};
		}
	}

	static Handle create(Class & obj)
	{
		static_assert(
			ptr_type_tag == HandlePtrTag::OwnedByOwner,
			"Cannot create a non-shared non-transferred handle for shared / transferred types");
		return reinterpret_cast<Handle>(&obj);
	}

	static Handle create(SharedPtr<Class> const & ptr)
	{
		static_assert(
			ptr_type_tag == HandlePtrTag::Shared,
			"Cannot create a shared handle for a non-shared type");
		return reinterpret_cast<Handle>(new SharedPtr<Class>{ptr});
	}

	static Handle create(SharedPtr<Class> && ptr)
	{
		static_assert(
			ptr_type_tag == HandlePtrTag::Shared,
			"Cannot create a shared handle for a non-shared type");
		return reinterpret_cast<Handle>(new SharedPtr<Class>{ptr});
	}

	static void release(Handle handle)
	{
		static_assert(
			ptr_type_tag != HandlePtrTag::OwnedByOwner,
			"Cannot release a handle not owned by client");
		static_assert(
			ptr_type_tag != HandlePtrTag::Temporary,
			"Cannot release a handle aliasing a temporary");

		if constexpr (ptr_type_tag == HandlePtrTag::Shared)
		{
			delete reinterpret_cast<SharedPtr<Class> *>(handle);
		}
		else if constexpr (ptr_type_tag == HandlePtrTag::OwnedByClient)
		{
			delete reinterpret_cast<Class *>(handle);
		}
	}

	template <class Handle>
	static PtrRef convert(Handle && handle)
	{
		if constexpr (
			ptr_type_tag == HandlePtrTag::OwnedByClient ||
			ptr_type_tag == HandlePtrTag::OwnedByOwner)
		{
			return reinterpret_cast<Class *>(handle);
		}
		else if constexpr (ptr_type_tag == HandlePtrTag::Temporary)
		{
			return &handle;
		}
		else if constexpr (ptr_type_tag == HandlePtrTag::Shared)
		{
			return *reinterpret_cast<SharedPtr<Class> *>(handle);
		}
	}

	template <class Ret, class Fn, class... Args>
	static fp_ErrorCode mem_fn(
		Fn && fn, fp_ErrorMessage err, Ret * out, Handle handle, Args... args)
	{
		return wrap_exception(
			err,
			[handle, &out, &fn, &args...]
			{
				*out = HandleFactory<Ret, THandleMap>::make(
					fn(*convert(handle),
					   *HandleFactory<Args, THandleMap>::convert(std::forward<Args>(args))...));
			});
	}

	template <class Fn, class... Args>
	static fp_ErrorCode mem_fn(Fn && fn, fp_ErrorMessage err, Handle handle, Args... args)
	{
		return wrap_exception(
			err,
			[&fn, handle, &args...] {
				fn(*convert(handle),
				   *HandleFactory<Args, THandleMap>::convert(std::forward<Args>(args))...);
			});
	}

	template <class Fn, class... Args>
	static auto mem_fn(Fn && fn, Handle handle, Args... args)
		-> std::invoke_result_t<Fn, Class, Args...>
	{
		return fn(
			*convert(handle),
			*HandleFactory<Args, THandleMap>::convert(std::forward<Args>(args))...);
	}
};

}  // namespace feltplugin::owner