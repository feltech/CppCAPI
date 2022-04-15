#include <memory>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <feltpluginsystem/interface.h>
#include <feltpluginsystem/plugin_definition.hpp>
#include <feltpluginsystem/service/handle_map.hpp>

using trompeloeil::_;

using StubOwnedByServiceHandle = struct StubOwnedByService_t *;
using StubOwnedByClientHandle = struct StubOwnedByClient_t *;
using StubSharedHandle = struct StubShared_t *;

struct Stub
{
};

using MockAPIOwnedByServiceHandle = struct MockAPIOwnedByService_t *;
using MockAPIOwnedByClientHandle = struct MockAPIOwnedByClient_t *;
using MockAPISharedHandle = struct MockAPIShared_t *;

template <typename>
struct MockAPI
{
	MAKE_CONST_MOCK0(no_return_no_error_no_out_no_args, void());
	MAKE_CONST_MOCK6(
		no_return_no_error_no_out_with_args, void(int, Stub &, float, Stub &, bool, Stub &));
};

template <class Handle>
struct MockAPISuite
{
	void (*no_return_no_error_no_out_no_args)(Handle);
	void (*no_return_no_error_no_out_with_args)(
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);
};

using MockAPIPlugin = feltplugin::PluginDefinition<
	// Service
	feltplugin::service::HandleMap<
		// MockAPI:
		// Owned by service.
		feltplugin::service::HandleTraits<
			MockAPIOwnedByServiceHandle,
			MockAPI<MockAPIOwnedByServiceHandle>,
			feltplugin::service::HandleOwnershipTag::OwnedByService>,
		// Owned by client.
		feltplugin::service::HandleTraits<
			MockAPIOwnedByClientHandle,
			MockAPI<MockAPIOwnedByClientHandle>,
			feltplugin::service::HandleOwnershipTag::OwnedByClient>,
		// Shared between service and client.
		feltplugin::service::HandleTraits<
			MockAPISharedHandle,
			MockAPI<MockAPISharedHandle>,
			feltplugin::service::HandleOwnershipTag::Shared>,

		// Stub:
		// Owned by service.
		feltplugin::service::HandleTraits<
			StubOwnedByServiceHandle,
			Stub,
			feltplugin::service::HandleOwnershipTag::OwnedByService>,
		// Owned by client.
		feltplugin::service::HandleTraits<
			StubOwnedByClientHandle,
			Stub,
			feltplugin::service::HandleOwnershipTag::OwnedByClient>,
		// Shared between service and client.
		feltplugin::service::
			HandleTraits<StubSharedHandle, Stub, feltplugin::service::HandleOwnershipTag::Shared>>>;

TEMPLATE_TEST_CASE(
	"Decorating C++ functions for a C function pointer suite",
	"",
	MockAPIOwnedByServiceHandle,
	MockAPIOwnedByClientHandle,
	MockAPISharedHandle)
{
	GIVEN("A C++ service type its C client handle and function suite")
	{
		using Handle = TestType;  // Magic from Catch2 TEMPLATE_TEST_CASE
		using MockAPI = MockAPI<Handle>;
		using Suite = MockAPISuite<Handle>;
		using SuiteDecorator = typename MockAPIPlugin::SuiteDecorator<Handle>;

		Handle handle;
		MockAPI * service_api;

		using DescriptionAndSuite = std::pair<std::string_view, Suite>;

		auto const [description, suite] = GENERATE(
			// Lambdas
			DescriptionAndSuite{
				"lambdas",
				{
					// no_return_no_error_no_out_no_args
					SuiteDecorator::decorate([](MockAPI & api)
											 { api.no_return_no_error_no_out_no_args(); }),

					// no_return_no_error_no_out_primitive_args
					SuiteDecorator::decorate(
						[](MockAPI & api, int i, Stub & s1, float f, Stub & s2, bool b, Stub & s3)
						{ api.no_return_no_error_no_out_with_args(i, s1, f, s2, b, s3); }),

				}},

			// Member functions.
			DescriptionAndSuite{
				"member functions",
				{// no_return_no_error_no_out_no_args
				 SuiteDecorator::decorate(SuiteDecorator::template mem_fn_ptr<
										  &MockAPI::no_return_no_error_no_out_no_args>),

				 // no_return_no_error_no_out_primitive_args
				 SuiteDecorator::decorate(SuiteDecorator::template mem_fn_ptr<
										  &MockAPI::no_return_no_error_no_out_with_args>)}});

		INFO("Suite type: " << description);

		if constexpr (std::is_same_v<Handle, MockAPIOwnedByServiceHandle>)
		{
			service_api = new MockAPI;
			handle = MockAPIPlugin::HandleManager<Handle>::create(*service_api);
		}
		else if constexpr (std::is_same_v<Handle, MockAPIOwnedByClientHandle>)
		{
			handle = MockAPIPlugin::HandleManager<Handle>::make_handle();
			service_api = MockAPIPlugin::HandleManager<Handle>::convert(handle);
		}
		else if constexpr (std::is_same_v<Handle, MockAPISharedHandle>)
		{
			handle = MockAPIPlugin::HandleManager<Handle>::make_handle();
			service_api = MockAPIPlugin::HandleManager<Handle>::convert(handle).get();
		}

		AND_GIVEN("no_return_no_error_no_out_no_args service function expects to be called")
		{
			REQUIRE_CALL(*service_api, no_return_no_error_no_out_no_args());

			WHEN("the corresponding suite function is called")
			{
				suite.no_return_no_error_no_out_no_args(handle);

				THEN("service function was called") {}
			}
		}

		AND_GIVEN("stub instances and associated handles to use as convertible parameters")
		{
			// Stub instance owned by the service.
			Stub stubOwnedByService{};
			StubOwnedByServiceHandle stubOwnedByServiceHandle =
				MockAPIPlugin::HandleManager<StubOwnedByServiceHandle>::create(stubOwnedByService);

			// Stub instance owned by the client.
			StubOwnedByClientHandle stubOwnedByClientHandle =
				MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::make_handle();
			Stub & stubOwnedByClient =
				*MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::convert(
					stubOwnedByClientHandle);

			// Stub instance shared between service and client.
			feltplugin::SharedPtr<Stub> ptrStubShared = feltplugin::make_shared<Stub>();
			StubSharedHandle stubSharedHandle =
				MockAPIPlugin::HandleManager<StubSharedHandle>::create(ptrStubShared);
			Stub & stubShared = *ptrStubShared;

			AND_GIVEN("no_return_no_error_no_out_with_args service function expects to be called")
			{
				REQUIRE_CALL(
					*service_api, no_return_no_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stubOwnedByService)
					.LR_WITH(&_4 == &stubOwnedByClient)
					.LR_WITH(&_6 == &stubShared);

				WHEN("the corresponding suite function is called")
				{
					suite.no_return_no_error_no_out_with_args(
						handle,
						123,
						stubOwnedByServiceHandle,
						0.234f,
						stubOwnedByClientHandle,
						true,
						stubSharedHandle);

					THEN("service function was called") {}
				}
			}

			MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(stubOwnedByClientHandle);
			MockAPIPlugin::HandleManager<StubSharedHandle>::release(stubSharedHandle);
		}
		if constexpr (std::is_same_v<Handle, MockAPIOwnedByServiceHandle>)
		{
			delete service_api;
		}
		else if constexpr (std::is_same_v<Handle, MockAPIOwnedByClientHandle>)
		{
			MockAPIPlugin::HandleManager<Handle>::release(handle);
		}
		else if constexpr (std::is_same_v<Handle, MockAPISharedHandle>)
		{
			MockAPIPlugin::HandleManager<Handle>::release(handle);
		}
	}
}
