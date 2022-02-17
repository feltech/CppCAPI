#pragma once

#include "../errors.hpp"
#include "../pointers.hpp"
#include "handle_map.hpp"

namespace feltplugin::owner
{

template <class THandle, class THandleMap>
struct HandleFactory
{
	using Handle = THandle;
	using Ptr = typename THandleMap::template ptr_from_handle<Handle>;
	using Class = typename THandleMap::template class_from_handle<Handle>;

	template <typename... Args>
	static fp_ErrorCode make(char * err, Handle * out, Args... args)
	{
		return wrap_exception(err, [&out, &args...] { *out = make(std::forward<Args>(args)...); });
	}

	template <typename... Args>
	static Handle make(Args &&... args)
	{
		if constexpr (std::is_same_v<Ptr, SharedPtr<Class>>)
		{
			return create(std::make_shared<Class>(std::forward<Args>(args)...));
		}
		else if constexpr (std::is_same_v<Ptr, UniquePtr<Class>>)
		{
			return create(std::make_unique<Class>(std::forward<Args>(args)...));
		}
		else if constexpr (std::is_same_v<Ptr, RawPtr<Class>>)
		{
			return reinterpret_cast<Handle>(new Class{std::forward<Args>(args)...});
		}
		// C-native type.
		else if constexpr (std::is_same_v<Class, Handle>)
		{
			return Class{std::forward<Args>(args)...};
		}
		else
		{
			static_assert(always_false_t<Args...>::value, "Unrecognized holder type");
		}
	}

	static Handle create(UniquePtr<Class> && ptr)
	{
		return reinterpret_cast<Handle>(new Ptr{std::move(ptr)});
	}

	static Handle create(SharedPtr<Class> const & ptr)
	{
		return reinterpret_cast<Handle>(new Ptr{ptr});
	}

	static Handle create(SharedPtr<Class> && ptr)
	{
		return reinterpret_cast<Handle>(new Ptr{ptr});
	}

	static void release(Handle handle)
	{
		delete reinterpret_cast<Ptr *>(handle);
	}

	template <class Handle, std::enable_if_t<!std::is_same_v<Class, Handle>, bool> = true>
	static Ptr & convert(Handle handle)
	{
		return *reinterpret_cast<Ptr *>(handle);
	}

	template <class Handle, std::enable_if_t<std::is_same_v<Class, Handle>, bool> = true>
	static Ptr convert(Handle && handle)
	{
		return &handle;
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
		-> std::invoke_result_t<Fn, Handle, Args...>
	{
		return fn(
			*convert(handle),
			*HandleFactory<Args, THandleMap>::convert(std::forward<Args>(args))...);
	}
};

}  // namespace feltplugin::owner