#pragma once

#include <feltplugin/errors.h>
#include <feltplugin/errors.hpp>

namespace feltplugin::client
{

template <class THandle, class THandleMap>
struct HandleAdapter
{
protected:
	using Base = HandleAdapter<THandle, THandleMap>;

public:
	using Handle = THandle;
	using Suite = typename THandleMap::template suite_from_handle<THandle>;
	using SuiteFactory = Suite (*)();

protected:
	static constexpr SuiteFactory ksuite_factory =
		THandleMap::template suite_factory_from_handle<THandle>();

public:
	explicit HandleAdapter(Handle handle) : HandleAdapter{handle, ksuite_factory}
	{
		static_assert(ksuite_factory != nullptr, "Attempting to construct with null suite factory");
	}

	explicit HandleAdapter(SuiteFactory suite_factory) : HandleAdapter{nullptr, suite_factory} {}

	explicit HandleAdapter(Handle handle, SuiteFactory suite_factory)
		: handle_{handle}, suite_{suite_factory()}
	{
	}

	HandleAdapter(HandleAdapter const &) = delete;
	HandleAdapter(HandleAdapter && other) noexcept : handle_{other.handle_}, suite_{other.suite_}
	{
		other.handle_ = nullptr;
	};

	virtual ~HandleAdapter()
	{
		suite_.release(handle_);
	}

	explicit operator Handle() const
	{
		return handle_;
	}

protected:
	HandleAdapter() : HandleAdapter{Handle{nullptr}} {}

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
}  // namespace feltplugin::receiver