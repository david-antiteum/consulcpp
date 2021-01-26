
#include "catch.hpp"
#include <consulcpp/ConsulCpp>

static constexpr char gKey[] = "my-key";
static constexpr char gVal[] = "my-val";

TEST_CASE( "KV operations", "[consul][kv]" ) {

	unsigned short port = 9990;

	consulcpp::Consul consul;

	REQUIRE( consul.connect() );

	SECTION( "Set and Get" ) {
		consul.kv().set( gKey, gVal );
		auto val = consul.kv().get( gKey );

		CHECK(( val && val.value() == gVal ));
	}
	SECTION( "Delete" ) {
		consul.kv().destroy( gKey );
		CHECK_FALSE( consul.kv().get( gKey ) );
	}
}
