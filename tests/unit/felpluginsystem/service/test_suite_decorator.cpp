#include <memory>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <feltpluginsystem/interface.h>
#include <feltpluginsystem/plugin_definition.hpp>
#include <feltpluginsystem/service/handle_map.hpp>

using trompeloeil::_;  // NOLINT(bugprone-reserved-identifier)

using StubOwnedByServiceHandle = struct StubOwnedByService_t *;
using StubOwnedByClientHandle = struct StubOwnedByClient_t *;
using StubSharedHandle = struct StubShared_t *;

struct Stub
{
	int value{0};
	bool operator==(Stub const & other) const
	{
		return other.value == value;
	}
};

using MockAPIOwnedByServiceHandle = struct MockAPIOwnedByService_t *;
using MockAPIOwnedByClientHandle = struct MockAPIOwnedByClient_t *;
using MockAPISharedHandle = struct MockAPIShared_t *;

struct MockAPI;

using MockAPIPlugin = feltplugin::PluginDefinition<
	// Service
	feltplugin::service::HandleMap<
		// MockAPI - bound to C API for signature tests:
		// Owned by service.
		feltplugin::service::HandleTraits<
			MockAPIOwnedByServiceHandle,
			MockAPI,
			feltplugin::service::HandleOwnershipTag::OwnedByService>,
		// Owned by client.
		feltplugin::service::HandleTraits<
			MockAPIOwnedByClientHandle,
			MockAPI,
			feltplugin::service::HandleOwnershipTag::OwnedByClient>,
		// Shared between service and client.
		feltplugin::service::HandleTraits<
			MockAPISharedHandle,
			MockAPI,
			feltplugin::service::HandleOwnershipTag::Shared>,

		// Stub - used as a (auto-converted) parameter in signature tests:
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

struct lambda_suite_t;
struct member_function_suite_t;

template <class, class>
struct MockAPISuiteImplFixture;

struct MockAPI
{
	MAKE_CONST_MOCK0(no_return_no_error_no_out_no_args, void());
	MAKE_CONST_MOCK6(
		no_return_no_error_no_out_with_args,
		void(int, Stub &, float, Stub &, bool, feltplugin::SharedPtr<Stub> &));
	MAKE_CONST_MOCK0(no_return_no_error_with_out_no_args, Stub());
	MAKE_CONST_MOCK6(
		no_return_no_error_with_out_with_args,
		Stub(int, Stub &, float, Stub &, bool, feltplugin::SharedPtr<Stub> &));
	MAKE_CONST_MOCK0(no_return_with_error_no_out_no_args, void());
	MAKE_CONST_MOCK6(
		no_return_with_error_no_out_with_args,
		void(int, Stub &, float, Stub &, bool, feltplugin::SharedPtr<Stub> &));
	// no_return_with_error_no_out_with_args
	// no_return_with_error_with_out_no_args
	// no_return_with_error_with_out_with_args
	MAKE_CONST_MOCK6(
		no_return_with_error_with_out_with_args,
		Stub(int, Stub &, float, Stub &, bool, feltplugin::SharedPtr<Stub> &));
	MAKE_CONST_MOCK0(with_return_no_error_no_out_no_args, int());
	MAKE_CONST_MOCK6(
		with_return_no_error_no_out_with_args,
		int(int, Stub &, float, Stub &, bool, feltplugin::SharedPtr<Stub> &));
	// with_return_no_error_with_out_no_args - N/A mutually exclusive: return/out
	// with_return_no_error_with_out_with_args - N/A mutually exclusive: return/out
	// with_return_with_error_no_out_no_args - N/A mutually exclusive: return/error
	// with_return_with_error_no_out_with_args - N/A mutually exclusive: return/error
	// with_return_with_error_with_out_no_args - N/A mutually exclusive: return/error/out
	// with_return_with_error_with_out_with_args - N/A mutually exclusive: return/error/out
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

	void (*no_return_no_error_with_out_no_args)(StubOwnedByClientHandle *, Handle);

	void (*no_return_no_error_with_out_with_args)(
		StubOwnedByClientHandle *,
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);

	fp_ErrorCode (*no_return_with_error_no_out_no_args)(fp_ErrorMessage *, Handle);

	fp_ErrorCode (*no_return_with_error_no_out_with_args)(
		fp_ErrorMessage *,
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);

	fp_ErrorCode (*no_return_with_error_with_out_with_args)(
		fp_ErrorMessage *,
		StubOwnedByClientHandle *,
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);

	int (*with_return_no_error_no_out_no_args)(Handle);

	int (*with_return_no_error_no_out_with_args)(
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);
};

template <class THandle>
struct MockAPISuiteImplFixture<THandle, lambda_suite_t>
{
	static constexpr std::string_view suite_type_name = "lambda";

	using Handle = THandle;
	using SuiteDecorator = typename MockAPIPlugin::SuiteDecorator<Handle>;

	MockAPISuite<Handle> const suite{
		// no_return_no_error_no_out_no_args
		SuiteDecorator::decorate([](MockAPI & api) { api.no_return_no_error_no_out_no_args(); }),

		// no_return_no_error_no_out_with_args
		SuiteDecorator::decorate([](MockAPI & api,
									int i,
									Stub & s1,
									float f,
									Stub & s2,
									bool b,
									feltplugin::SharedPtr<Stub> & s3)
								 { api.no_return_no_error_no_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_no_error_with_out_no_args
		SuiteDecorator::decorate([](MockAPI & api)
								 { return api.no_return_no_error_with_out_no_args(); }),

		// no_return_no_error_with_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ return api.no_return_no_error_with_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_with_error_no_out_no_args
		SuiteDecorator::decorate([](MockAPI & api) { api.no_return_with_error_no_out_no_args(); }),

		// no_return_with_error_no_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ api.no_return_with_error_no_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_with_error_with_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ return api.no_return_with_error_with_out_with_args(i, s1, f, s2, b, s3); }),

		// with_return_no_error_no_out_no_args
		SuiteDecorator::decorate([](MockAPI & api)
								 { return api.with_return_no_error_no_out_no_args(); }),

		// with_return_no_error_no_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ return api.with_return_no_error_no_out_with_args(i, s1, f, s2, b, s3); })};
};

template <>
struct MockAPISuiteImplFixture<MockAPISharedHandle, lambda_suite_t>
{
	static constexpr std::string_view suite_type_name = "lambda";

	using Handle = MockAPISharedHandle;
	using SuiteDecorator = typename MockAPIPlugin::SuiteDecorator<Handle>;

	MockAPISuite<Handle> const suite{
		// no_return_no_error_no_out_no_args
		SuiteDecorator::decorate([](feltplugin::SharedPtr<MockAPI> & api)
								 { api->no_return_no_error_no_out_no_args(); }),

		// no_return_no_error_no_out_with_args
		SuiteDecorator::decorate(
			[](feltplugin::SharedPtr<MockAPI> & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ api->no_return_no_error_no_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_no_error_with_out_no_args
		SuiteDecorator::decorate([](feltplugin::SharedPtr<MockAPI> & api)
								 { return api->no_return_no_error_with_out_no_args(); }),

		// no_return_no_error_with_out_with_args
		SuiteDecorator::decorate(
			[](feltplugin::SharedPtr<MockAPI> & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ return api->no_return_no_error_with_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_with_error_no_out_no_args
		SuiteDecorator::decorate([](feltplugin::SharedPtr<MockAPI> & api)
								 { api->no_return_with_error_no_out_no_args(); }),

		// no_return_with_error_no_out_with_args
		SuiteDecorator::decorate(
			[](feltplugin::SharedPtr<MockAPI> & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ api->no_return_with_error_no_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_with_error_with_out_with_args
		SuiteDecorator::decorate(
			[](feltplugin::SharedPtr<MockAPI> & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ return api->no_return_with_error_with_out_with_args(i, s1, f, s2, b, s3); }),

		// with_return_no_error_no_out_no_args
		SuiteDecorator::decorate([](feltplugin::SharedPtr<MockAPI> & api)
								 { return api->with_return_no_error_no_out_no_args(); }),

		// with_return_no_error_no_out_with_args
		SuiteDecorator::decorate(
			[](feltplugin::SharedPtr<MockAPI> & api,
			   int i,
			   Stub & s1,
			   float f,
			   Stub & s2,
			   bool b,
			   feltplugin::SharedPtr<Stub> & s3)
			{ return api->with_return_no_error_no_out_with_args(i, s1, f, s2, b, s3); })};
};

template <class THandle>
struct MockAPISuiteImplFixture<THandle, member_function_suite_t>
{
	static constexpr std::string_view suite_type_name = "member function";

	using Handle = THandle;
	using SuiteDecorator = typename MockAPIPlugin::SuiteDecorator<Handle>;

	MockAPISuite<Handle> const suite{
		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_no_error_no_out_no_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_no_error_no_out_with_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_no_error_with_out_no_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_no_error_with_out_with_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_with_error_no_out_no_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_with_error_no_out_with_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_with_error_with_out_with_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::with_return_no_error_no_out_no_args>),

		// with_return_no_error_no_out_with_args
		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::with_return_no_error_no_out_with_args>),

	};
};

template <class, class>
struct MockAPIFixture;

template <class suite_type>
struct MockAPIFixture<MockAPIOwnedByServiceHandle, suite_type>
	: MockAPISuiteImplFixture<MockAPIOwnedByServiceHandle, suite_type>
{
	using Handle =
		typename MockAPISuiteImplFixture<MockAPIOwnedByServiceHandle, suite_type>::Handle;

	MockAPI & service_api = *(new MockAPI);
	Handle handle = MockAPIPlugin::HandleManager<Handle>::create(service_api);

	~MockAPIFixture()
	{
		delete &service_api;
	}
};

template <class suite_type>
struct MockAPIFixture<MockAPIOwnedByClientHandle, suite_type>
	: MockAPISuiteImplFixture<MockAPIOwnedByClientHandle, suite_type>
{
	using Handle = typename MockAPISuiteImplFixture<MockAPIOwnedByClientHandle, suite_type>::Handle;

	Handle handle = MockAPIPlugin::HandleManager<Handle>::make_handle();
	MockAPI & service_api = MockAPIPlugin::HandleManager<Handle>::convert(handle);

	~MockAPIFixture()
	{
		MockAPIPlugin::HandleManager<Handle>::release(handle);
	}
};

template <class suite_type>
struct MockAPIFixture<MockAPISharedHandle, suite_type>
	: MockAPISuiteImplFixture<MockAPISharedHandle, suite_type>
{
	using Handle = typename MockAPISuiteImplFixture<MockAPISharedHandle, suite_type>::Handle;

	Handle handle = MockAPIPlugin::HandleManager<Handle>::make_handle();
	MockAPI & service_api = *MockAPIPlugin::HandleManager<Handle>::convert(handle);

	~MockAPIFixture()
	{
		MockAPIPlugin::HandleManager<Handle>::release(handle);
	}
};

template <class suite_type>
using owned_by_service_t = MockAPIFixture<MockAPIOwnedByServiceHandle, suite_type>;
template <class suite_type>
using owned_by_client_t = MockAPIFixture<MockAPIOwnedByClientHandle, suite_type>;
template <class suite_type>
using owned_by_shared_t = MockAPIFixture<MockAPISharedHandle, suite_type>;

TEMPLATE_PRODUCT_TEST_CASE(
	"Decorating C++ functions for a C function pointer suite",
	"",
	(owned_by_service_t, owned_by_client_t, owned_by_shared_t),
	(lambda_suite_t, member_function_suite_t))
{
	GIVEN("A C++ service type its C client handle and function suite")
	{
		TestType fixture;
		auto & handle = fixture.handle;
		auto & service_api = fixture.service_api;
		auto & suite = fixture.suite;

		INFO("Suite type: " << TestType::suite_type_name);

		AND_GIVEN("no_return_no_error_no_out_no_args service function expects to be called")
		{
			REQUIRE_CALL(service_api, no_return_no_error_no_out_no_args());

			WHEN("the corresponding suite function is called")
			{
				suite.no_return_no_error_no_out_no_args(handle);

				THEN("service function was called") {}
			}
		}

		AND_GIVEN("with_return_no_error_no_out_no_args service function expects to be called")
		{
			constexpr int expectedReturnValue = 123;
			REQUIRE_CALL(service_api, with_return_no_error_no_out_no_args())
				.RETURN(expectedReturnValue);

			WHEN("the corresponding suite function is called")
			{
				const int actualReturnValue = suite.with_return_no_error_no_out_no_args(handle);

				THEN("suite function returns expected value")
				{
					CHECK(actualReturnValue == expectedReturnValue);
				}
			}
		}

		AND_GIVEN("no_return_with_error_no_out_no_args service function expects to be called")
		{
			REQUIRE_CALL(service_api, no_return_with_error_no_out_no_args());
			WHEN("the corresponding suite function is called")
			{
				std::string storage(500, '\0');
				fp_ErrorMessage err{storage.size(), 0, storage.data()};
				fp_ErrorCode code = suite.no_return_with_error_no_out_no_args(&err, handle);

				THEN("error is OK")
				{
					CHECK(code == fp_ok);
					CHECK(std::string_view{err.data, err.size}.empty());
				}
			}
		}

		AND_GIVEN("no_return_with_error_no_out_no_args service function throws an exception")
		{
			REQUIRE_CALL(service_api, no_return_with_error_no_out_no_args())
				.THROW(std::domain_error{"Mock domain_error"});

			WHEN("the corresponding suite function is called")
			{
				std::string storage(500, '\0');
				fp_ErrorMessage err{storage.size(), 0, storage.data()};
				fp_ErrorCode code = suite.no_return_with_error_no_out_no_args(&err, handle);

				THEN("error is reported")
				{
					CHECK(code == fp_error);
					CHECK(std::string_view{err.data, err.size} == "Mock domain_error");
				}
			}
		}

		AND_GIVEN("no_return_no_error_with_out_no_args service function expects to be called")
		{
			Stub const expectedReturnValue{789};
			REQUIRE_CALL(service_api, no_return_no_error_with_out_no_args())
				.RETURN(expectedReturnValue);

			WHEN("the corresponding suite function is called")
			{
				StubOwnedByClientHandle actualReturnValue;
				suite.no_return_no_error_with_out_no_args(&actualReturnValue, handle);

				THEN("suite function returns expected value")
				{
					Stub const & actualUnpackedReturnValue =
						MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::convert(
							actualReturnValue);
					CHECK(actualUnpackedReturnValue == expectedReturnValue);
					// Check copied not just pointed to.
					CHECK(&actualUnpackedReturnValue != &expectedReturnValue);
				}

				MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(actualReturnValue);
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
				MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::convert(
					stubOwnedByClientHandle);

			// Stub instance shared between service and client.
			StubSharedHandle stubSharedHandle =
				MockAPIPlugin::HandleManager<StubSharedHandle>::make_handle();
			feltplugin::SharedPtr<Stub> & ptrStubShared =
				MockAPIPlugin::HandleManager<StubSharedHandle>::convert(stubSharedHandle);

			AND_GIVEN("no_return_no_error_no_out_with_args service function expects to be called")
			{
				REQUIRE_CALL(
					service_api, no_return_no_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stubOwnedByService)
					.LR_WITH(&_4 == &stubOwnedByClient)
					.LR_WITH(&_6 == &ptrStubShared);

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

			AND_GIVEN("no_return_no_error_with_out_with_args service function expects to be called")
			{
				Stub const expectedReturnValue{789};
				REQUIRE_CALL(
					service_api, no_return_no_error_with_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stubOwnedByService)
					.LR_WITH(&_4 == &stubOwnedByClient)
					.LR_WITH(&_6 == &ptrStubShared)
					.RETURN(expectedReturnValue);

				WHEN("the corresponding suite function is called")
				{
					StubOwnedByClientHandle actualReturnValue;
					suite.no_return_no_error_with_out_with_args(
						&actualReturnValue,
						handle,
						123,
						stubOwnedByServiceHandle,
						0.234f,
						stubOwnedByClientHandle,
						true,
						stubSharedHandle);

					THEN("suite function returns expected value")
					{
						Stub const & actualUnpackedReturnValue =
							MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::convert(
								actualReturnValue);
						CHECK(actualUnpackedReturnValue == expectedReturnValue);
						// Check copied not just pointed to.
						CHECK(&actualUnpackedReturnValue != &expectedReturnValue);
					}

					MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(
						actualReturnValue);
				}
			}

			AND_GIVEN("no_return_with_error_no_out_with_args service function expects to be called")
			{
				REQUIRE_CALL(
					service_api, no_return_with_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stubOwnedByService)
					.LR_WITH(&_4 == &stubOwnedByClient)
					.LR_WITH(&_6 == &ptrStubShared);

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					fp_ErrorMessage err{storage.size(), 0, storage.data()};

					fp_ErrorCode code = suite.no_return_with_error_no_out_with_args(
						&err,
						handle,
						123,
						stubOwnedByServiceHandle,
						0.234f,
						stubOwnedByClientHandle,
						true,
						stubSharedHandle);

					THEN("error is OK")
					{
						CHECK(code == fp_ok);
						CHECK(std::string_view{err.data, err.size}.empty());
					}
				}
			}

			AND_GIVEN(
				"no_return_with_error_with_out_with_args service function expects to be called")
			{
				Stub const expectedReturnValue{789};
				REQUIRE_CALL(
					service_api,
					no_return_with_error_with_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stubOwnedByService)
					.LR_WITH(&_4 == &stubOwnedByClient)
					.LR_WITH(&_6 == &ptrStubShared)
					.RETURN(expectedReturnValue);

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					fp_ErrorMessage err{storage.size(), 0, storage.data()};
					StubOwnedByClientHandle actualReturnValue;

					fp_ErrorCode code = suite.no_return_with_error_with_out_with_args(
						&err,
						&actualReturnValue,
						handle,
						123,
						stubOwnedByServiceHandle,
						0.234f,
						stubOwnedByClientHandle,
						true,
						stubSharedHandle);

					THEN("suite function returns expected value")
					{
						// No error.
						CHECK(code == fp_ok);
						CHECK(std::string_view{err.data, err.size}.empty());
						// Return value.
						Stub const & actualUnpackedReturnValue =
							MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::convert(
								actualReturnValue);
						CHECK(actualUnpackedReturnValue == expectedReturnValue);
						// Check copied not just pointed to.
						CHECK(&actualUnpackedReturnValue != &expectedReturnValue);
					}

					MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(
						actualReturnValue);
				}
			}

			AND_GIVEN("with_return_no_error_no_out_with_args service function expects to be called")
			{
				constexpr int expectedReturnValue = 123;

				REQUIRE_CALL(
					service_api, with_return_no_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stubOwnedByService)
					.LR_WITH(&_4 == &stubOwnedByClient)
					.LR_WITH(&_6 == &ptrStubShared)
					.RETURN(expectedReturnValue);

				WHEN("the corresponding suite function is called")
				{
					const int actualReturnValue = suite.with_return_no_error_no_out_with_args(
						handle,
						123,
						stubOwnedByServiceHandle,
						0.234f,
						stubOwnedByClientHandle,
						true,
						stubSharedHandle);

					THEN("suite function returns expected value")
					{
						CHECK(actualReturnValue == expectedReturnValue);
					}
				}
			}

			MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(stubOwnedByClientHandle);
			MockAPIPlugin::HandleManager<StubSharedHandle>::release(stubSharedHandle);
		}
	}
}
