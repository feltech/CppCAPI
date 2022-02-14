#pragma once

#include <feltplugin/handles.h>
#include <feltplugin/errors.hpp>

namespace feltplugin::plugin
{

template <class HandleTraits>
struct HandleAdapter
{
	using Handle = typename HandleTraits::Handle;
	using Suite = typename HandleTraits::Suite;
	using Class = typename HandleTraits::Class;
	static constexpr auto get_suite = HandleTraits::get_suite;

	explicit HandleAdapter() : handle_{nullptr}, suite_{get_suite()} {}
	explicit HandleAdapter(Handle handle) : handle_{handle}, suite_{get_suite()} {}
	HandleAdapter(HandleAdapter const &) = delete;
	HandleAdapter(HandleAdapter &&) noexcept = default;
	~HandleAdapter() = default;

	explicit operator Handle() const
	{
		return handle_;
	}

	template <class... Args>
	void create(Args &&... args)
	{
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = suite_.create(err, &handle_, std::forward<Args>(args)...);
		throw_on_error(code, err);
	}

	template <class Ret, class... Args, class... Rest>
	Ret call(fp_ErrorCode (*fn)(fp_ErrorMessage, Ret *, Handle, Args...), Rest &&... args)
	{
		Ret ret;
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = fn(err, &ret, handle_, std::forward<Rest>(args)...);
		throw_on_error(code, err);
		return ret;
	}

	template <class... Args, class... Rest>
	void call(fp_ErrorCode (*fn)(fp_ErrorMessage, Handle, Args...), Rest &&... args)
	{
		fp_ErrorCode code;
		fp_ErrorMessage err;

		code = fn(err, handle_, std::forward<Rest>(args)...);
		throw_on_error(code, err);
	}

protected:
	Handle handle_;
	Suite const suite_;
};
}  // namespace feltplugin