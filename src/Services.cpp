
#include "consulcpp/Services.h"
#include "consulcpp/Service.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

struct consulcpp::Services::Private
{
	consulcpp::Consul & mConsul;

	explicit Private( Consul & consul )
		: mConsul( consul )
	{
	}

	std::string api() const
	{
		return fmt::format( "{}/{}/agent/service", mConsul.agentAddress(), mConsul.agentAPIVersion() );
	}
};

consulcpp::Services::Services( Consul & consul )
	: d( std::make_unique<Private>( consul ) )
{
}

consulcpp::Services::~Services() = default;

void consulcpp::Services::create( const Service & service ) const
{
	nlohmann::json serviceAsJson = service;
	spdlog::debug( "REGISTER {}", serviceAsJson.dump() );
	if( auto response = internal::HttpClient::put( fmt::format( "{}/register", d->api() ), service ); response ){
		spdlog::debug( "consulcpp::Services::create response: {}", response.value() );
	}else{
		spdlog::error( "consulcpp::Services::create error: {}", response.error() );
	}
}

void consulcpp::Services::destroy( const Service & service ) const
{
	if( auto response = internal::HttpClient::put( fmt::format( "{}/deregister/{}", d->api(), service.id() ) ); response ){
		spdlog::debug( "consulcpp::Services::destroy response: {}", response.value() );
	}else{
		spdlog::error( "consulcpp::Services::destroy error: {}", response.error() );
	}
}

std::optional<consulcpp::Service> consulcpp::Services::find( std::string_view id ) const
{
	std::optional<consulcpp::Service> res;

	if( auto response = internal::HttpClient::get( fmt::format( "{}/{}", d->api(), id ) ); response ) {
		try{
			nlohmann::json json = nlohmann::json::parse( response.value() );
			res					= json.get<consulcpp::Service>();
		} catch( const std::exception & e ) {
			spdlog::error( "consulcpp::Services::find error: {}. Response was: {}", e.what(), response.value() );
		}
	}
	return res;
}
