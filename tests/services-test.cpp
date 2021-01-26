
#include "catch.hpp"
#include <consulcpp/ConsulCpp>

TEST_CASE( "Services operations", "[consul][services]" ) {

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
}
