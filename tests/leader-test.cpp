
#include "catch.hpp"
#include <consulcpp/ConsulCpp>

TEST_CASE( "Leader operations", "[consul][leader]" ) {

	unsigned short port = 9990;

	consulcpp::Consul consul;

	REQUIRE( consul.connect() );

	SECTION( "Acquire and release" ) {
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
}
