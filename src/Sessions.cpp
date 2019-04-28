
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
	internal::HttpClient	restClient;
	Session					res;

	auto jsonMaybe = restClient.put( fmt::format( "{}/{}/session/create", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion() ), {} );
	if( jsonMaybe ){
		res.mId = jsonMaybe.value()["ID"];
	}
	return res;
}

void consulcpp::Sessions::destroy( const consulcpp::Session & session )
{
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.put( fmt::format( "{}/{}/session/destroy/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), session.mId ), {} );

}
