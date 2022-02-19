#pragma once
#include <memory>

namespace feltplugin
{
template <class Class>
using SharedPtr = std::shared_ptr<Class>;

template <class Class, typename... Args>
SharedPtr<Class> make_shared(Args &&... args)
{
	return std::make_shared<Class>(std::forward<Args>(args)...);
}
}  // namespace feltplugin