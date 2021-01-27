
#include "consulcpp/Sessions.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

struct consulcpp::Sessions::Private
{
	consulcpp::Consul & mConsul;

	explicit Private( Consul & consul )
		: mConsul( consul )
	{
	}

	std::string api() const
	{
		return fmt::format( "{}/{}/session", mConsul.agentAddress(), mConsul.agentAPIVersion() );
	}
};

consulcpp::Sessions::Sessions( Consul & consul )
	: d( spimpl::make_impl<Private>( consul ) )
{
}

consulcpp::Session consulcpp::Sessions::create() const
{
	Session res;

	if( auto response = internal::HttpClient::put( fmt::format( "{}/create", d->api() ) ); response ) {
		try{
			auto jsonValue = nlohmann::json::parse( response.value() );
			res.mId		   = jsonValue[ "ID" ];
		} catch( const nlohmann::json::parse_error & e ) {
			spdlog::error( "consulcpp::Sessions::create parser error: {}. Response was: {}", e.what(), response.value() );
		} catch( const std::exception & e ) {
			spdlog::error( "consulcpp::Sessions::create error: {}. Response was: {}", e.what(), response.value() );
		}
	}
	return res;
}

bool consulcpp::Sessions::destroy( const consulcpp::Session & session ) const
{
	bool res = false;

	if( auto response = internal::HttpClient::put( fmt::format( "{}/destroy/{}", d->api(), session.mId ) ); response ) {
		res = response.value().find( "true" ) != std::string::npos;
	} else {
		spdlog::error( "KV delete: Consul returns the error {}", response.error() );
	}
	return res;
}
