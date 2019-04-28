
#include "consulcpp/Services.h"
#include "consulcpp/Service.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

struct consulcpp::Services::Private
{
	consulcpp::Consul		& mConsul;

	Private( Consul & consul )
		: mConsul( consul )
	{
	}
};

consulcpp::Services::Services( Consul & consul ) : d( std::make_unique<Private>( consul ) )
{

}

consulcpp::Services::~Services()
{
}

void consulcpp::Services::publish( const Service & service )
{
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.put( fmt::format( "{}/{}/agent/service/register", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion() ), service );
}

void consulcpp::Services::unpublish( const Service & service )
{
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.put( fmt::format( "{}/{}/agent/service/deregister/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), service.mId ), {} );

}
