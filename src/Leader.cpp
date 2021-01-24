
#include "consulcpp/Leader.h"
#include "consulcpp/Sessions.h"
#include "consulcpp/Services.h"
#include "consulcpp/Service.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

struct consulcpp::Leader::Private
{
	consulcpp::Consul & mConsul;

	explicit Private( Consul & consul )
		: mConsul( consul )
	{
	}

	std::string api() const
	{
		return fmt::format( "{}/{}/kv/service", mConsul.agentAddress(), mConsul.agentAPIVersion() );
	}
};

consulcpp::Leader::Leader( Consul & consul )
	: d( std::make_unique<Private>( consul ) )
{
}

consulcpp::Leader::~Leader() = default;

consulcpp::Leader::Status consulcpp::Leader::acquire( const Service & service, const Session & session ) const
{
	Leader::Status res	 = Status::Error;
	std::string	   query = fmt::format( "{}/{}/leader?acquire={}", d->api(), service.mName, session.mId );

	if( auto response = consulcpp::internal::HttpClient::put( query ); response ) {
		if( response.value().find( "true" ) != std::string::npos ) {
			res = Status::Yes;
		} else if( response.value().find( "false" ) != std::string::npos ) {
			res = Status::No;
		}
	} else {
		spdlog::error( "Leader acquire: Consul returns the error {}", response.error() );
	}
	return res;
}

void consulcpp::Leader::release( const Service & service, const Session & session ) const
{
	std::string query = fmt::format( "{}/{}/leader?release={}", d->api(), service.mName, session.mId );

	if( auto response = consulcpp::internal::HttpClient::put( query ); response ) {
		if( response.value().find( "true" ) == std::string::npos ) {
			spdlog::warn( "Leader release: release fails. Value was not acquire by this service?" );
		}
	} else {
		spdlog::error( "Leader release: Consul returns the error {}", response.error() );
	}
}
