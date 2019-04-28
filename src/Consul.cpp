#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "consulcpp/Consul.h"
#include "consulcpp/Services.h"
#include "consulcpp/Sessions.h"
#include "consulcpp/Leader.h"

#include "internal/HttpClient.h"

struct consulcpp::Consul::Private
{
	std::string 			mAgentAddress;
	std::string 			mAgentAPIVersion = "v1";
	std::string				mAddress;
	consulcpp::Services		mServices;
	consulcpp::Sessions		mSessions;
	consulcpp::Leader		mLeader;

	Private( Consul & consul, const std::string & agentAddress )
		: mServices( consul )
		, mSessions( consul )
		, mLeader( consul )
	{
		if( agentAddress.empty() ){
			mAgentAddress = "http://127.0.0.1:8500";
		}else{
			mAgentAddress = agentAddress;
		}
	}
};

consulcpp::Consul::Consul() : d( std::make_unique<Private>( *this, std::string() ) )
{

}

consulcpp::Consul::Consul( const std::string & agentAddress ) : d( std::make_unique<Private>( *this, agentAddress ) )
{

}

consulcpp::Consul::~Consul()
{
}

bool consulcpp::Consul::connect()
{
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.get( fmt::format( "{}/{}/agent/self", d->mAgentAddress, d->mAgentAPIVersion ) );
	if( jsonMaybe ){
		auto jsonValue = jsonMaybe.value();
		try{
			auto memberJson = jsonValue.at( "Member" );
			d->mAddress = memberJson.at( "Addr" ).get<std::string>();
		}catch( const std::exception & e ){
			spdlog::error( "{}. Json was: {}", e.what(), jsonValue.dump() );
		}
	}
	return !d->mAddress.empty();
}

std::string consulcpp::Consul::address() const
{
	return d->mAddress;
}

std::string consulcpp::Consul::agentAddress() const
{
	return d->mAgentAddress;
}

std::string consulcpp::Consul::agentAPIVersion() const
{
	return d->mAgentAPIVersion;
}

consulcpp::Services & consulcpp::Consul::services() const
{
	return d->mServices;
}

consulcpp::Sessions & consulcpp::Consul::sessions() const
{
	return d->mSessions;
}

consulcpp::Leader & consulcpp::Consul::leader() const
{
	return d->mLeader;
}
