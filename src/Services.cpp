
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
	spdlog::info( "REGISTER {}", serviceAsJson.dump() );
	if( auto response = internal::HttpClient::put( fmt::format( "{}/register", d->api() ), service ); response ){
		spdlog::info( "consulcpp::Services::create response: {}", response.value() );
	}else{
		spdlog::error( "consulcpp::Services::create error: {}", response.error() );
	}
}

void consulcpp::Services::destroy( const Service & service ) const
{
	if( auto response = internal::HttpClient::put( fmt::format( "{}/deregister/{}", d->api(), service.id() ) ); response ){
		spdlog::info( "consulcpp::Services::destroy response: {}", response.value() );
	}else{
		spdlog::error( "consulcpp::Services::destroy error: {}", response.error() );
	}
}

std::optional<consulcpp::Service> consulcpp::Services::findInLocal( std::string_view id ) const
{
	std::optional<consulcpp::Service> res;

	if( auto response = internal::HttpClient::get( fmt::format( "{}/{}", d->api(), id ) ); response ) {
		try{
			nlohmann::json json = response.value();
			res					= json.get<consulcpp::Service>();
		} catch( const std::exception & e ) {
			spdlog::error( "consulcpp::Services::findInLocal error: {}. Response was: {}", e.what(), response.value() );
		}
	}
	return res;
}

std::vector<consulcpp::Service> consulcpp::Services::findInCatalog( std::string_view name, const std::vector<std::string> & tags ) const
{
	std::vector<consulcpp::Service> res;
	std::string						path = fmt::format( "{}/{}", d->api(), name );
	std::string						query;

	if( !tags.empty() ) {
		for( const std::string & tag: tags ) {
			if( query.empty() ) {
				query += fmt::format( "tag={}", tag );
			} else {
				query += fmt::format( "&tag={}", tag );
			}
		}
	}
	if( !query.empty() ) {
		path += "?" + query;
	}
	if( auto response = consulcpp::internal::HttpClient::get( path ); response ) {
		try{
			auto jsonValue = nlohmann::json::parse( response.value() );
			for( auto val: jsonValue ) {
				res.push_back( val.get<consulcpp::Service>() );
			}
		} catch( const std::exception & e ) {
			spdlog::error( "consulcpp::Services::findInCatalog error: {}. Response was: {}", e.what(), response.value() );
		}
	}
	return res;
}
