#pragma once

#include "../pointers.hpp"
#include "../handle_map.hpp"
#include "../errors.hpp"

namespace feltplugin::owner
{

template <class Handle, class HandleMap>
struct HandleFactory
{
	using Ptr = typename HandleMap::template ptr_from_handle<Handle>;
	using Class = typename HandleMap::template class_from_handle<Handle>;

	template <typename... Args>
	static fp_ErrorCode make(fp_ErrorMessage err, Handle * out, Args &&... args)
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
		else
		{
			return reinterpret_cast<Handle>(new Class{std::forward<Args>(args)...});
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

	static void release(Handle handle)
	{
		delete reinterpret_cast<Ptr *>(handle);
	}

	static Ptr & convert(Handle handle)
	{
		return *reinterpret_cast<Ptr *>(handle);
	}
};
}