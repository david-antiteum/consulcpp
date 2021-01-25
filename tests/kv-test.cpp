
#include "catch.hpp"
#include <consulcpp/ConsulCpp>

TEST_CASE( "creation of services", "[consul]" ) {

	unsigned short port = 9990;

	consulcpp::Consul consul;

    REQUIRE( consul.connect() );

    SECTION( "Check service in catalog" ) {
		consulcpp::Service		service;

		service.mId	     = "test-001";
		service.mName	 = "test-001";
		service.mAddress = consul.address();
		service.mPort    = port;

		consul.services().create( service );

        CHECK( consul.services().find( service.mId ) );
        CHECK_FALSE( consul.catalog().find( service.mId, {} ).empty() );

		consul.services().destroy( service );
    }

    SECTION( "Check several services in catalog" ) {
		consulcpp::Service		service1;

		service1.mId	  = "test-002.1";
		service1.mName    = "test-002";
		service1.mAddress = consul.address();
		service1.mPort    = port;

		consulcpp::Service		service2;

		service2.mId	  = "test-002.2";
		service2.mName    = "test-002";
		service2.mAddress = consul.address();
		service2.mPort    = port + 1;

		consul.services().create( service1 );
		consul.services().create( service2 );

        CHECK( consul.services().find( service1.mId ) );
        CHECK( consul.services().find( service2.mId ) );
        CHECK( consul.catalog().find( service1.mName, {} ).size() == 2 );

		consul.services().destroy( service2 );
		consul.services().destroy( service1 );
	}

    SECTION( "Check leader" ) {
		consulcpp::Service		service1;

		service1.mId	  = "test-002.1";
		service1.mName    = "test-002";
		service1.mAddress = consul.address();
		service1.mPort    = port;

		consulcpp::Service		service2;

		service2.mId	  = "test-002.2";
		service2.mName    = "test-002";
		service2.mAddress = consul.address();
		service2.mPort    = port + 1;

		consul.services().create( service1 );
		consul.services().create( service2 );

		consulcpp::Session session1 = consul.sessions().create();

        CHECK_FALSE( session1.mId.empty() );

		auto leader1 = consul.leader().acquire( service1, session1 );

		CHECK( leader1 == consulcpp::Leader::Status::Yes );

		consulcpp::Session session2 = consul.sessions().create();

        CHECK_FALSE( session2.mId.empty() );

		auto leader2 = consul.leader().acquire( service2, session2 );

		CHECK( leader2 == consulcpp::Leader::Status::No );

		consul.leader().release( service1, session1 );

		leader2 = consul.leader().acquire( service2, session2 );

		CHECK( leader2 == consulcpp::Leader::Status::Yes );

		consul.sessions().destroy( session2 );
		consul.services().destroy( service2 );

		consul.sessions().destroy( session1 );
		consul.services().destroy( service1 );
	}

    SECTION( "Check KV" ) {
		consul.kv().set( "my-key", "my-value" );
		auto val = consul.kv().get( "my-key" );

		CHECK( (val && val.value() == "my-value") );

		consul.kv().destroy( "my-key" );

		CHECK_FALSE( consul.kv().get( "my-key" ) );
	}
}
