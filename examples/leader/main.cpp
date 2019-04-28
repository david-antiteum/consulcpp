
#include <consulcpp/Consul.h>
#include <consulcpp/Service.h>
#include <consulcpp/Services.h>
#include <consulcpp/Sessions.h>
#include <consulcpp/Leader.h>

#include <fmt/printf.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <chrono>
#include <thread>

#include "../common/helpers.h"

int main( int argc, char * argv[] )
{
	consulcpp::Consul		consul;
	
	if( consul.connect() ){
		spdlog::info( "Agent address {}", consul.address() );

		consulcpp::Service			service;
		consulcpp::ServiceCheck		check;

		service.mId = fmt::format( "{}", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() );
		service.mName = "leader-demo";
		service.mAddress = consul.address();
		service.mPort = 11000;

		check.mHTTP = fmt::format( "http://{}:{}/health", service.mAddress, service.mPort );
		check.mInterval = "5s";
		service.mChecks = { check };

		consul.services().publish( service );

		consulcpp::Session 	session = consul.sessions().create();
		spdlog::info( "Session {}", session.mId );

		consulcpp::Leader::Status leader = consul.leader().acquire( service, session );
		if( leader == consulcpp::Leader::Status::Yes ){
			spdlog::info( "I'm the leader" );
		}else{
			spdlog::info( "I'm a follower" );
		}
		loop();

		consul.leader().release( service, session );
		consul.sessions().destroy( session );
		consul.services().unpublish( service );
		spdlog::info( "Bye" );
	}
	return 0;
}
