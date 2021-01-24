
#include <consulcpp/ConsulCpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <chrono>
#include <thread>
#include <optional>

#include "../common/helpers.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
DISABLE_WARNING_DLL_INTERFACE
DISABLE_WARNING_LOSS_OF_DATA
#include <grpc++/grpc++.h>
#include "health.grpc.pb.h"
DISABLE_WARNING_POP

class HealthServiceImpl final : public grpc::health::v1::Health::Service
{
	grpc::Status Check( grpc::ServerContext * context, const grpc::health::v1::HealthCheckRequest * request, grpc::health::v1::HealthCheckResponse * reply ) override
	{
		reply->set_status( grpc::health::v1::HealthCheckResponse_ServingStatus_SERVING );
		spdlog::info( "HealthServiceImpl Check called" );
		return grpc::Status::OK;
	}
};

void TestService( consulcpp::Service & service )
{
	std::string			serverAddress = fmt::format( "{}:{}", service.mAddress, service.mPort );
	HealthServiceImpl	healthService;
	grpc::ServerBuilder builder;

	builder.AddListeningPort( serverAddress, grpc::InsecureServerCredentials() );
	builder.RegisterService( &healthService );

	if( std::unique_ptr<grpc::Server> server = builder.BuildAndStart(); server ){
		std::thread grpcThread( &grpc::Server::Wait, server.get() );
		spdlog::info( "Server listening on {}:{}", service.mAddress, service.mPort );
		loop();
		server->Shutdown();
		grpcThread.join();
		spdlog::info( "Server stopped" );
	} else {
		spdlog::critical( "Cannot start gRPC server on {}:{}", service.mAddress, service.mPort );
	}
}
int main( int argc, char * argv[] )
{
	unsigned short port = 50051;

	if( argc > 1 ) {
		if( auto maybePort = consulcpp::utils::asPort( argv[1] ); maybePort ){
			port = maybePort.value();
		}else{
			spdlog::error( "This is not a valid port number {}", argv[1] );
			return 1;
		}
	}
	if( consulcpp::Consul consul; consul.connect() ) {
		spdlog::info( "Agent address {}", consul.address() );

		consulcpp::Service		service;
		consulcpp::ServiceCheck check;

		service.mId		 = fmt::format( "{}", std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count() );
		service.mName	 = "leader-demo";
		service.mAddress = consul.address();
		service.mPort    = port;
	
		check.mId							  = "presence";
		check.mInterval						  = "5s";
		check.mDeregisterCriticalServiceAfter = "1m";
		check.mGRPC							  = fmt::format( "{}:{}/Health", service.mAddress, service.mPort );
		service.mChecks						  = { check };

		// Purge death services
		std::vector<consulcpp::Service> otherServices = consul.services().findInCatalog( service.mName, service.mTags );
		for( const auto & otherService: otherServices ) {
			consul.services().destroy( otherService );
		}

		consul.services().create( service );
		consulcpp::Session session = consul.sessions().create();
		spdlog::info( "Session {}", session.mId );

		consul.kv().set( "my-key", "my-value" );

		if( consulcpp::Leader::Status leader = consul.leader().acquire( service, session ); leader == consulcpp::Leader::Status::Yes ) {
			spdlog::info( "I'm the leader" );
		} else {
			spdlog::info( "I'm a follower" );
		}

		TestService( service );

		if( auto val = consul.kv().get( "my-key" ); val ) {
			spdlog::info( "Key value was {}", val.value() );
			if( consul.kv().destroy( "my-key" ) ) {
				spdlog::info( "Key deleted" );
			}
		} else {
			spdlog::info( "Key not found" );
		}
		consul.leader().release( service, session );
		consul.sessions().destroy( session );
		consul.services().destroy( service );
		spdlog::info( "Bye" );
	}
	return 0;
}
