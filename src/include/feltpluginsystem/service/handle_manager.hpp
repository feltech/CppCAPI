#pragma once
/**
 * Contains the HandleManager to be used by services converting to/from opaque
 * handles
 */

#include <cstring>
#include <functional>

#include "../error_map.hpp"
#include "../interface.h"
#include "../pointers.hpp"
#include "handle_map.hpp"

namespace feltplugin::service
{

/**
 * Utility to static_assert that a given handle maps to either a native class or adapter.
 *
 * If the Handle type is not found in the service::HandleMap, then Class resolves to Handle
 * (pass-through). If the Handle type is not found in the client::HandleMap, then the
 * Adapter resolves to `std::false_type`. These two facts can be used to assert that the Handle
 * is in the service xor client handle map.
 *
 * @tparam Handle Handle type to check.
 * @tparam Class Native class type that the handle is associated with, if any.
 * @tparam Adapter Adapter class that the handle is associated with, if any.
 */
template <class Handle, class Class, class Adapter>
constexpr void assert_is_valid_handle_type()
{
	static_assert(
		!(std::is_same_v<Handle, Class> && std::is_same_v<Adapter, std::false_type>),
		"Attempting to wrap a handle that is unrecognized. Are you missing an entry in your "
		"HandleMap lists?");
};

/**
 * Utility class for creating and converting opaque handles.
 *
 * @tparam THandle Opaque handle type.
 * @tparam TServiceHandleMap service::HandleMap for mapping handles to native classes.
 * @tparam TClientHandleMap client::HandleMap for mapping handles to client adapter classes.
 * @tparam TErrorMap ErrorMap for mapping exceptions to error codes.
 */
template <class THandle, class TServiceHandleMap, class TClientHandleMap, class TErrorMap>
struct HandleManager
{
private:
	using Handle = THandle;
	using Class = typename TServiceHandleMap::template class_from_handle<Handle>;
	using Adapter = typename TClientHandleMap::template class_from_handle<Handle>;
	static constexpr HandleOwnershipTag ptr_type_tag =
		TServiceHandleMap::template ownersihp_tag_from_handle<Handle>();

public:
	static constexpr bool is_service_handle()
	{
		return ptr_type_tag != HandleOwnershipTag::Unrecognized;
	}

	static constexpr bool is_client_handle()
	{
		return !std::is_same_v<Adapter, std::false_type>;
	}

	/**
	 * Convert an opaque handle to a concrete instance.
	 *
	 * The behaviour of this function varies depending on `ptr_type_tag` in our HandleTraits.
	 * However, (a reference to) a concrete object is always returned. This can be either the
	 * original object that is associated with the handle, or a client adapter class that wraps the
	 * handle, or failing that will pass through the handle unconverted (i.e. assume it's a native
	 * C type).
	 *
	 * @tparam Handle Type of handle. Required to enable forwarding references.
	 * @param handle Opaque handle to convert.
	 * @return Dereferenceable object that dereferences to the original object associated with the
	 * opaque handle.
	 */
	template <class Handle>
	static decltype(auto) convert(Handle && handle)
	{
		if constexpr (is_service_handle())
		{
			if constexpr (
				ptr_type_tag == HandleOwnershipTag::OwnedByClient ||
				ptr_type_tag == HandleOwnershipTag::OwnedByService)
			{
				return *reinterpret_cast<Class *>(handle);
			}
			else if constexpr (ptr_type_tag == HandleOwnershipTag::Shared)
			{
				return **reinterpret_cast<SharedPtr<Class> *>(handle);
			}
			throw std::logic_error("Unhandled handle ownership");
		}
		else if constexpr (is_client_handle())
		{
			// Client handle type.
			return Adapter{handle};
		}
		else
		{
			// Native C type.
			return std::forward<Handle>(handle);
		}
	}

	/**
	 * Construct a new instance of our Class type and associate it with a Handle.
	 *
	 * Ownership is determined by the `ptr_type_tag` enum value in the HandleTraits for our Handle.
	 *
	 * This function is not valid if the HandlePtrTag is `OwnedByService`, since that implies
	 * a handle should be associated with an existing object rather than creating a new one.
	 *
	 * @tparam Args Argument types to pass to the constructor.
	 * @param args Arguments to pass to the constructor.
	 * @return Newly minted opaque handle.
	 */
	template <typename... Args>
	static Handle make_handle(Args &&... args)
	{
		static_assert(
			ptr_type_tag != HandleOwnershipTag::OwnedByService,
			"Cannot make a handle to a new instance for non-shared non-transferred types");
		static_assert(
			!is_client_handle(),
			"Cannot make a handle to a new instance from the client.");

		if constexpr (ptr_type_tag == HandleOwnershipTag::Shared)
		{
			return create(feltplugin::make_shared<Class>(std::forward<Args>(args)...));
		}
		else if constexpr (ptr_type_tag == HandleOwnershipTag::OwnedByClient)
		{
			return reinterpret_cast<Handle>(new Class{std::forward<Args>(args)...});
		}
		// Native type.
		else if constexpr (ptr_type_tag == HandleOwnershipTag::Unrecognized)
		{
			return Class{std::forward<Args>(args)...};
		}
	}
	/**
	 * Create a handle associated with a pre-existing instance.
	 *
	 * This function is only valid if the HandleOwnershipTag in the HandleTraits is
	 * `OwnedByService`.
	 *
	 * @param obj Object to reference.
	 * @return Newly minted opaque handle.
	 */
	static Handle create(Class & obj)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();
		static_assert(
			ptr_type_tag == HandleOwnershipTag::OwnedByService,
			"Cannot create a non-shared non-transferred handle for shared / transferred types");
		return reinterpret_cast<Handle>(&obj);
	}

	/**
	 * Create a handle associated with a pre-existing shared smart pointer to an instance.
	 *
	 * This function is only valid if the `ptr_type_tag` in the HandleTraits is `Shared`.
	 *
	 * The given shared pointer's reference count will be incremented and not decremented again
	 * until `release` is called.
	 *
	 * @param ptr Pointer to wrap.
	 * @return Newly minted opaque handle.
	 */
	static Handle create(SharedPtr<Class> const & ptr)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();
		static_assert(
			ptr_type_tag == HandleOwnershipTag::Shared,
			"Cannot create a shared handle for a non-shared type");
		return reinterpret_cast<Handle>(new SharedPtr<Class>{ptr});
	}

	/**
	 * Create a handle associated with a pre-existing shared smart pointer to an instance.
	 *
	 * This function is only valid if the `ptr_type_tag` in the HandleTraits is `Shared`.
	 *
	 * The given shared pointer's reference count will be incremented and not decremented again
	 * until `release` is called.
	 *
	 * @param ptr Pointer to wrap.
	 * @return Newly minted opaque handle.
	 */
	static Handle create(SharedPtr<Class> && ptr)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();
		static_assert(
			ptr_type_tag == HandleOwnershipTag::Shared,
			"Cannot create a shared handle for a non-shared type");
		return reinterpret_cast<Handle>(new SharedPtr<Class>{ptr});
	}

	/**
	 * Construct a new instance of our Class type and associate it with a Handle.
	 *
	 * The signature of this function matches the convention that function pointer suites should
	 * adhere to, allowing it to be used directly without wrapping, e.g. when defining the `create`
	 * member during construction of a function pointer suite we can simply do
	 * `.create = &Converter::make_handle,`.
	 *
	 * @tparam Args Argument types to pass to the constructor.
	 * @param[out] err Storage for exception message, if one occurs during construction.
	 * @param[out] out Pointer to handle to newly constructed object.
	 * @param args Arguments to pass to the constructor.
	 * @return Error code.
	 */
	template <typename... Args>
	static fp_ErrorCode make(fp_ErrorMessage * err, Handle * out, Args... args)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();
		return TErrorMap::wrap_exception(*err, [&out, &args...] { *out = make_handle(args...); });
	}

	/**
	 * Release an opaque handle.
	 *
	 * This function is only valid if the `ptr_type_tag` in our HandleTraits is `OwnedByClient` or
	 * `Shared`.
	 *
	 * If `OwnedByClient` then the object is destroyed. If `Shared` then the reference count is
	 * decremented, potentially destroying the object.
	 *
	 * @param handle Handle to release.
	 */
	static void release(Handle handle)
	{
		assert_is_valid_handle_type<Handle, Class, Adapter>();
		static_assert(
			ptr_type_tag != HandleOwnershipTag::OwnedByService,
			"Cannot release a handle not owned by client");
		static_assert(
			ptr_type_tag != HandleOwnershipTag::Unrecognized,
			"Cannot release a handle aliasing a temporary. Are you missing an entry in your "
			"HandleMaps?");

		if constexpr (ptr_type_tag == HandleOwnershipTag::Shared)
		{
			delete reinterpret_cast<SharedPtr<Class> *>(handle);
		}
		else if constexpr (ptr_type_tag == HandleOwnershipTag::OwnedByClient)
		{
			delete reinterpret_cast<Class *>(handle);
		}
	}
};
}  // namespace feltplugin::service