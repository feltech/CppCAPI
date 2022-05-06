// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Pointer types used internally.
 *
 * Currently just adds wrappers for `shared_ptr`. The wrappers should be used by preference in case
 * tweaks are added in the future.
 */
#pragma once
#include <memory>

namespace cppcapi
{
/// Wrap `shared_ptr`.
template <class Class>
using SharedPtr = std::shared_ptr<Class>;

/// Wrap `std::make_shared`.
template <class Class, typename... Args>
SharedPtr<Class> make_shared(Args &&... args)
{
	return std::make_shared<Class>(std::forward<Args>(args)...);
}
}  // namespace cppcapi