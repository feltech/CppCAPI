#include <catch2/catch.hpp>
#include <memory>
#include <trompeloeil.hpp>

#include <feltpluginsystem/interface.h>
#include <feltpluginsystem/plugin_definition.hpp>
#include <feltpluginsystem/service/handle_map.hpp>

class MockAPI
{
public:
	MAKE_CONST_MOCK0(simplest_possible, void());
};

using FakeOwnedByServiceHandle = struct FakeOwnedByService_t *;
using FakeOwnedByClientHandle = struct FakeOwnedByClient_t *;
using FakeSharedHandle = struct FakeShared_t *;

template <class Handle>
struct FakeSuite
{
	void (*simplest_possible)(Handle);
};

using FakePlugin = feltplugin::PluginDefinition<
	// Service
	feltplugin::service::HandleMap<
		// Owned by service.
		feltplugin::service::HandleTraits<
			FakeOwnedByServiceHandle,
			MockAPI,
			feltplugin::service::HandleOwnershipTag::OwnedByService>,
		// Owned by client.
		feltplugin::service::HandleTraits<
			FakeOwnedByClientHandle,
			MockAPI,
			feltplugin::service::HandleOwnershipTag::OwnedByClient>,
		// Shared between service and client..
		feltplugin::service::
			HandleTraits<FakeSharedHandle, MockAPI, feltplugin::service::HandleOwnershipTag::Shared>

		>>;

TEMPLATE_TEST_CASE(
	"Decorating C++ functions for a C function pointer suite",
	"",
	FakeOwnedByServiceHandle,
	FakeOwnedByClientHandle,
	FakeSharedHandle)
{
	GIVEN("A C++ service type its C client handle and function suite")
	{
		using Handle = TestType;  // Magic from Catch2 TEMPLATE_TEST_CASE

		Handle handle;
		MockAPI * service_api;

		if constexpr (std::is_same_v<Handle, FakeOwnedByServiceHandle>)
		{
			service_api = new MockAPI;
			handle = FakePlugin::HandleManager<Handle>::create(*service_api);
		}
		else if constexpr (std::is_same_v<Handle, FakeOwnedByClientHandle>)
		{
			handle = FakePlugin::HandleManager<Handle>::make_handle();
			service_api = FakePlugin::HandleManager<Handle>::convert(handle);
		}
		else if constexpr (std::is_same_v<Handle, FakeSharedHandle>)
		{
			handle = FakePlugin::HandleManager<Handle>::make_handle();
			service_api = FakePlugin::HandleManager<Handle>::convert(handle).get();
		}

		FakeSuite<Handle> suite{// simplest_possible
								FakePlugin::SuiteDecorator<Handle>::decorate(
									[](MockAPI const & api) { api.simplest_possible(); })};

		AND_GIVEN("simplest_possible service function expects to be called")
		{
			REQUIRE_CALL(*service_api, simplest_possible());

			WHEN("the corresponding suite function is called")
			{
				suite.simplest_possible(handle);

				THEN("service function was called") {}
			}
		}

		if constexpr (std::is_same_v<Handle, FakeOwnedByServiceHandle>)
		{
			delete service_api;
		}
		else if constexpr (std::is_same_v<Handle, FakeOwnedByClientHandle>)
		{
			FakePlugin::HandleManager<Handle>::release(handle);
		}
		else if constexpr (std::is_same_v<Handle, FakeSharedHandle>)
		{
			FakePlugin::HandleManager<Handle>::release(handle);
		}
	}
}
