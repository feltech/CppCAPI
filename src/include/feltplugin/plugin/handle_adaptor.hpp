#pragma once

#include <feltplugin/handles.h>
#include <feltplugin/exceptions.hpp>

namespace feltplugin::plugin
{

template <class Handle>
struct HandleAdapter
{
	explicit HandleAdapter(Handle h) : handle{h} {}
	HandleAdapter(HandleAdapter const &) = delete;
	HandleAdapter(HandleAdapter &&) noexcept = default;

	explicit operator Handle() const
	{
		return handle;
	}

	Handle handle;

	template <class... Args, class... Rest>
	static Handle create(fp_ErrorCode (*fn)(fp_ErrorMessage, Handle *, Args...), Rest &&... args)
	{
		Handle handle;
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = fn(err, &handle, std::forward<Rest>(args)...);
		throw_on_error(code, err);
		return handle;
	}

	template <class Ret, class... Args, class... Rest>
	Ret call(fp_ErrorCode (*fn)(fp_ErrorMessage, Ret *, Handle, Args...), Rest &&... args)
	{
		Ret ret;
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = fn(err, &ret, handle, std::forward<Rest>(args)...);
		throw_on_error(code, err);
		return ret;
	}

	template <class... Args, class... Rest>
	void call(fp_ErrorCode (*fn)(fp_ErrorMessage, Handle, Args...), Rest &&... args)
	{
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = fn(err, handle, std::forward<Rest>(args)...);
		throw_on_error(code, err);
	}
};
}  // namespace feltplugin