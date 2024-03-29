#include <memory>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <cppcapi/interface.h>
#include <cppcapi/plugin_definition.hpp>
#include <cppcapi/pointers.hpp>
#include <cppcapi/service/handle_map.hpp>

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

using MockAPIPlugin = cppcapi::PluginDefinition<
	// Service
	cppcapi::service::HandleMap<
		// MockAPI - bound to C API for signature tests:
		// Owned by service.
		cppcapi::service::HandleTraits<
			MockAPIOwnedByServiceHandle,
			MockAPI,
			cppcapi::service::HandleOwnershipTag::OwnedByService>,
		// Owned by client.
		cppcapi::service::HandleTraits<
			MockAPIOwnedByClientHandle,
			MockAPI,
			cppcapi::service::HandleOwnershipTag::OwnedByClient>,
		// Shared between service and client.
		cppcapi::service::HandleTraits<
			MockAPISharedHandle,
			MockAPI,
			cppcapi::service::HandleOwnershipTag::Shared>,

		// Stub - used as a (auto-converted) parameter in signature tests:
		// Owned by service.
		cppcapi::service::HandleTraits<
			StubOwnedByServiceHandle,
			Stub,
			cppcapi::service::HandleOwnershipTag::OwnedByService>,
		// Owned by client.
		cppcapi::service::HandleTraits<
			StubOwnedByClientHandle,
			Stub,
			cppcapi::service::HandleOwnershipTag::OwnedByClient>,
		// Shared between service and client.
		cppcapi::service::
			HandleTraits<StubSharedHandle, Stub, cppcapi::service::HandleOwnershipTag::Shared>>>;

struct lambda_suite_t;
struct member_function_suite_t;

template <class, class>
struct MockAPISuiteImplFixture;

struct MockAPI
{
	MAKE_CONST_MOCK0(no_return_no_error_no_out_no_args, void());
	MAKE_CONST_MOCK6(
		no_return_no_error_no_out_with_args, void(int, Stub &, float, Stub &, bool, Stub &));
	// OwnedByShared return type.
	MAKE_CONST_MOCK0(no_return_no_error_with_out_no_args, Stub());
	// OwnedByService return type.
	MAKE_CONST_MOCK6(
		no_return_no_error_with_out_with_args, Stub &(int, Stub &, float, Stub &, bool, Stub &));
	MAKE_CONST_MOCK0(no_return_with_error_no_out_no_args, void());
	MAKE_CONST_MOCK6(
		no_return_with_error_no_out_with_args, void(int, Stub &, float, Stub &, bool, Stub &));
	// OwnedByClient return type.
	MAKE_CONST_MOCK0(no_return_with_error_with_out_no_args, Stub());
	// OwnedByClient return type.
	MAKE_CONST_MOCK6(
		no_return_with_error_with_out_with_args, Stub(int, Stub &, float, Stub &, bool, Stub &));
	MAKE_CONST_MOCK0(with_return_no_error_no_out_no_args, int());
	MAKE_CONST_MOCK0(with_sharedhandle_return_no_error_no_out_no_args, cppcapi::SharedPtr<Stub>());
	MAKE_CONST_MOCK0(with_servicehandle_return_no_error_no_out_no_args, Stub&());
	MAKE_CONST_MOCK6(
		with_return_no_error_no_out_with_args, int(int, Stub &, float, Stub &, bool, Stub &));
	MAKE_CONST_MOCK1(
		with_clienthandle_return_no_error_no_out_with_sharedptr_args,
		Stub(cppcapi::SharedPtr<Stub>));
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

	void (*no_return_no_error_with_out_no_args)(StubSharedHandle *, Handle);
	void (*no_return_no_error_with_out_with_args)(
		StubOwnedByServiceHandle *,
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);

	cppcapi_ErrorCode (*no_return_with_error_no_out_no_args)(cppcapi_ErrorMessage *, Handle);

	cppcapi_ErrorCode (*no_return_with_error_no_out_with_args)(
		cppcapi_ErrorMessage *,
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);

	cppcapi_ErrorCode (*no_return_with_error_with_out_no_args)(
		cppcapi_ErrorMessage *, StubOwnedByClientHandle *, Handle);

	cppcapi_ErrorCode (*no_return_with_error_with_out_with_args)(
		cppcapi_ErrorMessage *,
		StubOwnedByClientHandle *,
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);

	int (*with_return_no_error_no_out_no_args)(Handle);

	StubSharedHandle (*with_sharedhandle_return_no_error_no_out_no_args)(Handle);

	StubOwnedByServiceHandle (*with_servicehandle_return_no_error_no_out_no_args)(Handle);

	int (*with_return_no_error_no_out_with_args)(
		Handle,
		int,
		StubOwnedByServiceHandle,
		float,
		StubOwnedByClientHandle,
		bool,
		StubSharedHandle);

	StubOwnedByClientHandle (*with_clienthandle_return_no_error_no_out_with_sharedptr_args)(
		Handle, StubSharedHandle);
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
		SuiteDecorator::decorate(
			[](MockAPI & api, int i, Stub & s1, float f, Stub & s2, bool b, Stub & s3)
			{ api.no_return_no_error_no_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_no_error_with_out_no_args
		SuiteDecorator::decorate([](MockAPI & api)
								 { return api.no_return_no_error_with_out_no_args(); }),

		// no_return_no_error_with_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api, int i, Stub & s1, float f, Stub & s2, bool b, Stub & s3)
				-> decltype(auto)
			{ return api.no_return_no_error_with_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_with_error_no_out_no_args
		SuiteDecorator::decorate([](MockAPI & api) { api.no_return_with_error_no_out_no_args(); }),

		// no_return_with_error_no_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api, int i, Stub & s1, float f, Stub & s2, bool b, Stub & s3)
			{ api.no_return_with_error_no_out_with_args(i, s1, f, s2, b, s3); }),

		// no_return_with_error_with_out_no_args
		SuiteDecorator::decorate([](MockAPI & api)
								 { return api.no_return_with_error_with_out_no_args(); }),

		// no_return_with_error_with_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api, int i, Stub & s1, float f, Stub & s2, bool b, Stub & s3)
			{ return api.no_return_with_error_with_out_with_args(i, s1, f, s2, b, s3); }),

		// with_return_no_error_no_out_no_args
		SuiteDecorator::decorate([](MockAPI & api)
								 { return api.with_return_no_error_no_out_no_args(); }),

		// with_sharedhandle_return_no_error_no_out_no_args
		SuiteDecorator::template decorate<StubSharedHandle>(
			[](MockAPI & api) { return api.with_sharedhandle_return_no_error_no_out_no_args(); }),

		// with_servicehandle_return_no_error_no_out_no_args
		SuiteDecorator::template decorate<StubOwnedByServiceHandle>(
			[](MockAPI & api) -> decltype(auto)
			{ return api.with_servicehandle_return_no_error_no_out_no_args(); }),

		// with_return_no_error_no_out_with_args
		SuiteDecorator::decorate(
			[](MockAPI & api, int i, Stub & s1, float f, Stub & s2, bool b, Stub & s3)
			{ return api.with_return_no_error_no_out_with_args(i, s1, f, s2, b, s3); }),

		// with_clienthandle_return_no_error_no_out_with_sharedptr_args
		SuiteDecorator::template decorate<StubOwnedByClientHandle>(
			[](MockAPI & api, const cppcapi::SharedPtr<Stub> & ptr)
			{ return api.with_clienthandle_return_no_error_no_out_with_sharedptr_args(ptr); }),

	};
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
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_with_error_with_out_no_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::no_return_with_error_with_out_with_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::with_return_no_error_no_out_no_args>),

		SuiteDecorator::template decorate<StubSharedHandle>(
			SuiteDecorator::template mem_fn_ptr<
				&MockAPI::with_sharedhandle_return_no_error_no_out_no_args>),

		SuiteDecorator::template decorate<StubOwnedByServiceHandle>(
			SuiteDecorator::template mem_fn_ptr<
				&MockAPI::with_servicehandle_return_no_error_no_out_no_args>),

		SuiteDecorator::decorate(
			SuiteDecorator::template mem_fn_ptr<&MockAPI::with_return_no_error_no_out_with_args>),

		SuiteDecorator::template decorate<StubOwnedByClientHandle>(
			SuiteDecorator::template mem_fn_ptr<
				&MockAPI::with_clienthandle_return_no_error_no_out_with_sharedptr_args>),

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

	MockAPI service_api{};
	Handle handle = MockAPIPlugin::HandleManager<Handle>::to_handle(service_api);
};

template <class suite_type>
struct MockAPIFixture<MockAPIOwnedByClientHandle, suite_type>
	: MockAPISuiteImplFixture<MockAPIOwnedByClientHandle, suite_type>
{
	using Handle = typename MockAPISuiteImplFixture<MockAPIOwnedByClientHandle, suite_type>::Handle;

	Handle handle = MockAPIPlugin::HandleManager<Handle>::make_to_handle();
	MockAPI & service_api = MockAPIPlugin::HandleManager<Handle>::to_instance(handle);

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

	Handle handle = MockAPIPlugin::HandleManager<Handle>::make_to_handle();
	MockAPI & service_api = MockAPIPlugin::HandleManager<Handle>::to_instance(handle);

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

/**
 * TODO(DF):
 * 	* Exception cases (possibly split into separate scenario).
 * 	* Exceptions of non std::exception type.
 * 	* boost::dll for loading plugins
 */
// Ignore warning coming from Catch2
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

TEMPLATE_PRODUCT_TEST_CASE(
	"Decorating C++ functions for a C function pointer suite",
	"",
	(owned_by_service_t, owned_by_client_t, owned_by_shared_t),
	(lambda_suite_t, member_function_suite_t))
{
#pragma GCC diagnostic pop
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

		AND_GIVEN("no_return_no_error_with_out_no_args service function expects to be called")
		{
			Stub const expected_return_value{789};
			REQUIRE_CALL(service_api, no_return_no_error_with_out_no_args())
				.RETURN(expected_return_value);

			WHEN("the corresponding suite function is called")
			{
				StubSharedHandle actual_return_value;
				suite.no_return_no_error_with_out_no_args(&actual_return_value, handle);

				THEN("suite function returns expected value")
				{
					Stub const & actual_unpacked_return_value =
						MockAPIPlugin::HandleManager<StubSharedHandle>::to_instance(
							actual_return_value);
					CHECK(actual_unpacked_return_value == expected_return_value);
					// Check copied not just pointed to.
					CHECK(&actual_unpacked_return_value != &expected_return_value);
				}

				MockAPIPlugin::HandleManager<StubSharedHandle>::release(actual_return_value);
			}
		}

		AND_GIVEN("no_return_with_error_no_out_no_args service function expects to be called")
		{
			REQUIRE_CALL(service_api, no_return_with_error_no_out_no_args());
			WHEN("the corresponding suite function is called")
			{
				std::string storage(500, '\0');
				cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};
				cppcapi_ErrorCode code = suite.no_return_with_error_no_out_no_args(&err, handle);

				THEN("error is OK")
				{
					CHECK(code == cppcapi_ok);
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
				cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};
				cppcapi_ErrorCode code = suite.no_return_with_error_no_out_no_args(&err, handle);

				THEN("error is reported")
				{
					CHECK(code == cppcapi_error);
					CHECK(std::string_view{err.data, err.size} == "Mock domain_error");
				}
			}
		}

		AND_GIVEN("with_return_no_error_no_out_no_args service function expects to be called")
		{
			constexpr int expected_return_value = 123;
			REQUIRE_CALL(service_api, with_return_no_error_no_out_no_args())
				.RETURN(expected_return_value);

			WHEN("the corresponding suite function is called")
			{
				const int actual_return_value = suite.with_return_no_error_no_out_no_args(handle);

				THEN("suite function returns expected value")
				{
					CHECK(actual_return_value == expected_return_value);
				}
			}
		}

		AND_GIVEN(
			"with_sharedhandle_return_no_error_no_out_no_args service function expects to be "
			"called")
		{
			cppcapi::SharedPtr<Stub> expected_return_value = cppcapi::make_shared<Stub>();
			REQUIRE_CALL(service_api, with_sharedhandle_return_no_error_no_out_no_args())
				.RETURN(expected_return_value);

			WHEN("the corresponding suite function is called")
			{
				StubSharedHandle actual_return_handle =
					suite.with_sharedhandle_return_no_error_no_out_no_args(handle);

				THEN("suite function returns expected pointer")
				{
					const cppcapi::SharedPtr<Stub> & actual_return_value =
						MockAPIPlugin::HandleManager<StubSharedHandle>::to_ptr(
							actual_return_handle);

					CHECK(actual_return_value.get() == expected_return_value.get());
				}

				MockAPIPlugin::HandleManager<StubSharedHandle>::release(actual_return_handle);
			}
		}

		AND_GIVEN(
			"with_servicehandle_return_no_error_no_out_no_args service function expects to be "
			"called")
		{
			Stub expected_return_value;

			REQUIRE_CALL(service_api, with_servicehandle_return_no_error_no_out_no_args())
				.LR_RETURN(std::ref(expected_return_value));

			WHEN("the corresponding suite function is called")
			{
				StubOwnedByServiceHandle actual_return_handle =
					suite.with_servicehandle_return_no_error_no_out_no_args(handle);

				THEN("suite function returns expected pointer")
				{
					const Stub & actual_return_value =
						MockAPIPlugin::HandleManager<StubOwnedByServiceHandle>::to_instance(
							actual_return_handle);

					CHECK(&actual_return_value == &expected_return_value);
				}
			}
		}

		AND_GIVEN("stub instances and associated handles to use as convertible parameters")
		{
			// Stub instance owned by the service.
			Stub stub_owned_by_service{};
			StubOwnedByServiceHandle handle_for_stub_owned_by_service =
				MockAPIPlugin::HandleManager<StubOwnedByServiceHandle>::to_handle(
					stub_owned_by_service);

			// Stub instance owned by the client.
			StubOwnedByClientHandle handle_for_stub_owned_by_client =
				MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::make_to_handle();
			Stub & stub_owned_by_client =
				MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::to_instance(
					handle_for_stub_owned_by_client);

			// Stub instance shared between service and client.
			StubSharedHandle handle_for_stub_owned_by_shared =
				MockAPIPlugin::HandleManager<StubSharedHandle>::make_to_handle();
			Stub & stub_owned_by_shared =
				MockAPIPlugin::HandleManager<StubSharedHandle>::to_instance(
					handle_for_stub_owned_by_shared);

			THEN("shared handle can be converted back to shared_ptr")
			{
				std::shared_ptr<Stub> ptr = MockAPIPlugin::HandleManager<StubSharedHandle>::to_ptr(
					handle_for_stub_owned_by_shared);
				CHECK(ptr.get() == &stub_owned_by_shared);
			}

			AND_GIVEN("no_return_no_error_no_out_with_args service function expects to be called")
			{
				REQUIRE_CALL(
					service_api, no_return_no_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stub_owned_by_service)
					.LR_WITH(&_4 == &stub_owned_by_client)
					.LR_WITH(&_6 == &stub_owned_by_shared);

				WHEN("the corresponding suite function is called")
				{
					suite.no_return_no_error_no_out_with_args(
						handle,
						123,
						handle_for_stub_owned_by_service,
						0.234f,
						handle_for_stub_owned_by_client,
						true,
						handle_for_stub_owned_by_shared);

					THEN("service function was called") {}
				}
			}

			AND_GIVEN("no_return_no_error_with_out_with_args service function expects to be called")
			{
				Stub expected_return_value{789};
				REQUIRE_CALL(
					service_api, no_return_no_error_with_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stub_owned_by_service)
					.LR_WITH(&_4 == &stub_owned_by_client)
					.LR_WITH(&_6 == &stub_owned_by_shared)
					.LR_RETURN(std::ref(expected_return_value));

				WHEN("the corresponding suite function is called")
				{
					StubOwnedByServiceHandle actual_return_value;
					suite.no_return_no_error_with_out_with_args(
						&actual_return_value,
						handle,
						123,
						handle_for_stub_owned_by_service,
						0.234f,
						handle_for_stub_owned_by_client,
						true,
						handle_for_stub_owned_by_shared);

					THEN("suite function returns expected value")
					{
						Stub const & actual_unpacked_return_value =
							MockAPIPlugin::HandleManager<StubOwnedByServiceHandle>::to_instance(
								actual_return_value);
						CHECK(actual_unpacked_return_value == expected_return_value);
						// Check points to same object.
						CHECK(&actual_unpacked_return_value == &expected_return_value);
					}
				}
			}

			AND_GIVEN("no_return_with_error_no_out_with_args service function expects to be called")
			{
				REQUIRE_CALL(
					service_api, no_return_with_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stub_owned_by_service)
					.LR_WITH(&_4 == &stub_owned_by_client)
					.LR_WITH(&_6 == &stub_owned_by_shared);

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};

					cppcapi_ErrorCode code = suite.no_return_with_error_no_out_with_args(
						&err,
						handle,
						123,
						handle_for_stub_owned_by_service,
						0.234f,
						handle_for_stub_owned_by_client,
						true,
						handle_for_stub_owned_by_shared);

					THEN("error is OK")
					{
						CHECK(code == cppcapi_ok);
						CHECK(std::string_view{err.data, err.size}.empty());
					}
				}
			}

			AND_GIVEN("no_return_with_error_no_out_with_args service function throws an exception")
			{
				REQUIRE_CALL(
					service_api, no_return_with_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stub_owned_by_service)
					.LR_WITH(&_4 == &stub_owned_by_client)
					.LR_WITH(&_6 == &stub_owned_by_shared)
					.THROW(std::domain_error{"Mock domain_error"});

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};

					cppcapi_ErrorCode code = suite.no_return_with_error_no_out_with_args(
						&err,
						handle,
						123,
						handle_for_stub_owned_by_service,
						0.234f,
						handle_for_stub_owned_by_client,
						true,
						handle_for_stub_owned_by_shared);

					THEN("error is reported")
					{
						// No error.
						CHECK(code == cppcapi_error);
						CHECK(std::string_view{err.data, err.size} == "Mock domain_error");
					}
				}
			}

			AND_GIVEN("no_return_with_error_with_out_no_args service function expects to be called")
			{
				Stub const expected_return_value{789};
				REQUIRE_CALL(service_api, no_return_with_error_with_out_no_args())
					.RETURN(expected_return_value);

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};
					StubOwnedByClientHandle actual_return_value;

					cppcapi_ErrorCode code = suite.no_return_with_error_with_out_no_args(
						&err, &actual_return_value, handle);

					THEN("suite function returns expected value")
					{
						// No error.
						CHECK(code == cppcapi_ok);
						CHECK(std::string_view{err.data, err.size}.empty());
						// Return value.
						Stub const & actual_unpacked_return_value =
							MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::to_instance(
								actual_return_value);
						CHECK(actual_unpacked_return_value == expected_return_value);
						// Check copied not just pointed to.
						CHECK(&actual_unpacked_return_value != &expected_return_value);
					}

					MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(
						actual_return_value);
				}
			}

			AND_GIVEN("no_return_with_error_with_out_no_args service function throws an exception")
			{
				REQUIRE_CALL(service_api, no_return_with_error_with_out_no_args())
					.THROW(std::domain_error{"Mock domain_error"});

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};
					StubOwnedByClientHandle actual_return_value;

					cppcapi_ErrorCode code = suite.no_return_with_error_with_out_no_args(
						&err, &actual_return_value, handle);

					THEN("suite function returns expected value")
					{
						// No error.
						CHECK(code == cppcapi_error);
						CHECK(std::string_view{err.data, err.size} == "Mock domain_error");
					}
				}
			}

			AND_GIVEN(
				"no_return_with_error_with_out_with_args service function expects to be called")
			{
				Stub const expected_return_value{789};
				REQUIRE_CALL(
					service_api,
					no_return_with_error_with_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stub_owned_by_service)
					.LR_WITH(&_4 == &stub_owned_by_client)
					.LR_WITH(&_6 == &stub_owned_by_shared)
					.RETURN(expected_return_value);

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};
					StubOwnedByClientHandle actual_return_value;

					cppcapi_ErrorCode code = suite.no_return_with_error_with_out_with_args(
						&err,
						&actual_return_value,
						handle,
						123,
						handle_for_stub_owned_by_service,
						0.234f,
						handle_for_stub_owned_by_client,
						true,
						handle_for_stub_owned_by_shared);

					THEN("suite function returns expected value")
					{
						// No error.
						CHECK(code == cppcapi_ok);
						CHECK(std::string_view{err.data, err.size}.empty());
						// Return value.
						Stub const & actual_unpacked_return_value =
							MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::to_instance(
								actual_return_value);
						CHECK(actual_unpacked_return_value == expected_return_value);
						// Check copied not just pointed to.
						CHECK(&actual_unpacked_return_value != &expected_return_value);
					}

					MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(
						actual_return_value);
				}
			}

			AND_GIVEN(
				"no_return_with_error_with_out_with_args service function throws an exception")
			{
				REQUIRE_CALL(
					service_api,
					no_return_with_error_with_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stub_owned_by_service)
					.LR_WITH(&_4 == &stub_owned_by_client)
					.LR_WITH(&_6 == &stub_owned_by_shared)
					.THROW(std::domain_error{"Mock domain_error"});

				WHEN("the corresponding suite function is called")
				{
					std::string storage(500, '\0');
					cppcapi_ErrorMessage err{storage.size(), 0, storage.data()};
					StubOwnedByClientHandle actual_return_value;

					cppcapi_ErrorCode code = suite.no_return_with_error_with_out_with_args(
						&err,
						&actual_return_value,
						handle,
						123,
						handle_for_stub_owned_by_service,
						0.234f,
						handle_for_stub_owned_by_client,
						true,
						handle_for_stub_owned_by_shared);

					THEN("suite function returns expected value")
					{
						// No error.
						CHECK(code == cppcapi_error);
						CHECK(std::string_view{err.data, err.size} == "Mock domain_error");
					}
				}
			}

			AND_GIVEN("with_return_no_error_no_out_with_args service function expects to be called")
			{
				constexpr int expected_return_value = 123;

				REQUIRE_CALL(
					service_api, with_return_no_error_no_out_with_args(123, _, 0.234f, _, true, _))
					.LR_WITH(&_2 == &stub_owned_by_service)
					.LR_WITH(&_4 == &stub_owned_by_client)
					.LR_WITH(&_6 == &stub_owned_by_shared)
					.RETURN(expected_return_value);

				WHEN("the corresponding suite function is called")
				{
					const int actual_return_value = suite.with_return_no_error_no_out_with_args(
						handle,
						123,
						handle_for_stub_owned_by_service,
						0.234f,
						handle_for_stub_owned_by_client,
						true,
						handle_for_stub_owned_by_shared);

					THEN("suite function returns expected value")
					{
						CHECK(actual_return_value == expected_return_value);
					}
				}
			}

			AND_GIVEN(
				"with_clienthandle_return_no_error_no_out_with_sharedptr_args service function "
				"expects to be called")
			{
				auto ptr_to_stub_owned_by_shared =
					MockAPIPlugin::HandleManager<StubSharedHandle>::to_ptr(
						handle_for_stub_owned_by_shared);

				Stub expected_return_value{123};
				REQUIRE_CALL(
					service_api, with_clienthandle_return_no_error_no_out_with_sharedptr_args(_))
					.LR_WITH(_1.get() == ptr_to_stub_owned_by_shared.get())
					.RETURN(expected_return_value);

				WHEN("the corresponding suite function is called")
				{
					StubOwnedByClientHandle actual_return_handle =
						suite.with_clienthandle_return_no_error_no_out_with_sharedptr_args(
							handle, handle_for_stub_owned_by_shared);

					THEN("suite function returns expected value")
					{
						Stub const & actual_return_value =
							MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::to_instance(
								actual_return_handle);
						CHECK(actual_return_value == expected_return_value);
					}

					MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(
						actual_return_handle);
				}
			}

			MockAPIPlugin::HandleManager<StubOwnedByClientHandle>::release(
				handle_for_stub_owned_by_client);
			MockAPIPlugin::HandleManager<StubSharedHandle>::release(
				handle_for_stub_owned_by_shared);
		}
	}
}
