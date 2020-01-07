#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "consulcpp/Consul.h"
#include "consulcpp/Services.h"
#include "consulcpp/Sessions.h"
#include "consulcpp/Leader.h"
#include "consulcpp/KV.h"

#include "internal/HttpClient.h"

struct consulcpp::Consul::Private
{
	std::string mAgentAddress;
	std::string mAgentAPIVersion = "v1";
	std::string mAddress;
	Services	mServices;
	Sessions	mSessions;
	Leader		mLeader;
	KV			mKV;

	Private( Consul & consul, const std::string & agentAddress )
		: mServices( consul )
		, mSessions( consul )
		, mLeader( consul )
		, mKV( consul )
	{
		if( agentAddress.empty() ) {
			mAgentAddress = "http://127.0.0.1:8500";
		} else {
			mAgentAddress = agentAddress;
		}
	}
};

consulcpp::Consul::Consul()
	: d( std::make_unique<Private>( *this, std::string() ) )
{
}

consulcpp::Consul::Consul( const std::string & agentAddress )
	: d( std::make_unique<Private>( *this, agentAddress ) )
{
}

consulcpp::Consul::~Consul()
{
}

bool consulcpp::Consul::connect()
{
	auto response = consulcpp::internal::HttpClient::get( fmt::format( "{}/{}/agent/self", d->mAgentAddress, d->mAgentAPIVersion ) );
	if( response ) {
		auto jsonValue = nlohmann::json::parse( response.value() );
		try {
			auto memberJson = jsonValue.at( "Member" );
			d->mAddress		= memberJson.at( "Addr" ).get<std::string>();
		} catch( const std::exception & e ) {
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

consulcpp::KV & consulcpp::Consul::kv() const
{
	return d->mKV;
}
