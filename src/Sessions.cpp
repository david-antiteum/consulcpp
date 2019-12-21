
#include "consulcpp/Sessions.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

struct consulcpp::Sessions::Private
{
	consulcpp::Consul		& mConsul;

	Private( Consul & consul )
		: mConsul( consul )
	{
	}
};

consulcpp::Sessions::Sessions( Consul & consul ) : d( std::make_unique<Private>( consul ) )
{

}

consulcpp::Sessions::~Sessions()
{
}

consulcpp::Session consulcpp::Sessions::create() const
{
	Session		res;

	auto response = consulcpp::internal::HttpClient::put( fmt::format( "{}/{}/session/create", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion() ) );
	if( response ){
		auto jsonValue = nlohmann::json::parse( response.value() );
		res.mId = jsonValue["ID"];
	}
	return res;
}

bool consulcpp::Sessions::destroy( const consulcpp::Session & session ) const
{
	bool		res = false;

	auto response = consulcpp::internal::HttpClient::put( fmt::format( "{}/{}/session/destroy/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), session.mId ) );
	if( response ){
		if( response.value().find( "true" ) != std::string::npos ){
			res = true;
		}
	}else{
		spdlog::error( "KV delete: Consul returns the error {}", response.error() );
	}
	return res;

}
