
#include <consulcpp/ConsulCpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <chrono>
#include <thread>

#include "../common/helpers.h"

#include <grpc++/grpc++.h>
#include "health.grpc.pb.h"

class HealthServiceImpl final : public grpc::health::v1::Health::Service
{
	grpc::Status Check( grpc::ServerContext* context, const grpc::health::v1::HealthCheckRequest * request, grpc::health::v1::HealthCheckResponse * reply ) override
	{
		reply->set_status( grpc::health::v1::HealthCheckResponse_ServingStatus_SERVING );
		spdlog::info( "HealthServiceImpl Check called" );
		return grpc::Status::OK;
	}
};

static std::unique_ptr<grpc::Server> gServer;

void RunServer( const consulcpp::Service & service )
{
	std::string 		serverAddress = fmt::format( "{}:{}", service.mAddress, service.mPort );
	HealthServiceImpl 	healthService;
	grpc::ServerBuilder builder;

	builder.AddListeningPort( serverAddress, grpc::InsecureServerCredentials() );
	builder.RegisterService( &healthService );

	gServer = builder.BuildAndStart();
	if( gServer ){
		spdlog::info( "Server listening on {}", serverAddress );
		gServer->Wait();
		spdlog::info( "Server stopped" );
	}else{
		spdlog::critical( "Cannot start gRPC server on {}", serverAddress );
	}
}

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
		service.mPort = 50051;

		check.mInterval = "5s";
		check.mDeregisterCriticalServiceAfter = "1m";
		check.mGRPC = fmt::format( "{}:{}/Health", service.mAddress, service.mPort );
		service.mChecks = { check };

		// Purge death services
		std::vector<consulcpp::Service>	otherServices = consul.services().findInCatalog( service.mName, service.mTags );
		for( auto service: otherServices ){
			consul.services().destroy( service );
		}

		consul.services().create( service );
		consulcpp::Session 	session = consul.sessions().create();
		spdlog::info( "Session {}", session.mId );

		consul.kv().set( "my-key", "my-value" );

		consulcpp::Leader::Status leader = consul.leader().acquire( service, session );
		if( leader == consulcpp::Leader::Status::Yes ){
			spdlog::info( "I'm the leader" );
		}else{
			spdlog::info( "I'm a follower" );
		}
		std::thread grpcThread( RunServer, service );
		loop();
		if( gServer ){
			gServer->Shutdown();
		}
		grpcThread.join();
		
		auto val = consul.kv().get( "my-key" );
		if( val ){
			spdlog::info( "Key value was {}", val.value() );
			if( consul.kv().destroy( "my-key" ) ){
				spdlog::info( "Key deleted" );
			}
		}else{
			spdlog::info( "Key not found" );
		}
		consul.leader().release( service, session );
		consul.sessions().destroy( session );
		consul.services().destroy( service );
		spdlog::info( "Bye" );
	}
	return 0;
}
