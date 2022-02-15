#pragma once

#include <feltplugin/handles.h>
#include <feltplugin/errors.hpp>

namespace feltplugin::plugin
{

template <class THandle, class TSuite, TSuite (*Tget_suite)(), class TClass>
struct HandleTraits
{
	using Handle = THandle;
	using Suite = TSuite;
	using Class = TClass;
	static constexpr auto get_suite = Tget_suite;
};

template <class Traits>
struct HandleAdapter
{
protected:
	using Base = HandleAdapter<Traits>;
	using Handle = typename Traits::Handle;
	using Suite = typename Traits::Suite;
	using Class = typename Traits::Class;
	static constexpr auto get_suite = Traits::get_suite;

public:
	explicit HandleAdapter(Handle handle) : handle_{handle}, suite_{get_suite()} {}
	HandleAdapter(HandleAdapter const &) = delete;
	HandleAdapter(HandleAdapter &&) noexcept = default;

	virtual ~HandleAdapter()
	{
		suite_.release(handle_);
	}

	explicit operator Handle() const
	{
		return handle_;
	}

protected:
	HandleAdapter() : handle_{nullptr}, suite_{get_suite()} {}

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
}  // namespace feltplugin::plugin