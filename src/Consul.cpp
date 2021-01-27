#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "consulcpp/Consul.h"
#include "consulcpp/Services.h"
#include "consulcpp/Catalog.h"
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
	Catalog		mCatalog;
	Sessions	mSessions;
	Leader		mLeader;
	KV			mKV;

	explicit Private( Consul & consul )
		: mServices( consul )
		, mCatalog( consul )
		, mSessions( consul )
		, mLeader( consul )
		, mKV( consul )
	{
	}

	std::string api() const
	{
		return fmt::format( "{}/{}/agent", mAgentAddress, mAgentAPIVersion );
	}
};

consulcpp::Consul::Consul()
	: d( spimpl::make_impl<Private>( *this ) )
{
}

bool consulcpp::Consul::connect( std::string_view agentAddress )
{
	if( agentAddress.empty() ) {
		d->mAgentAddress = "http://127.0.0.1:8500";
	} else {
		d->mAgentAddress = agentAddress;
	}
	if( auto response = consulcpp::internal::HttpClient::get( fmt::format( "{}/self", d->api() ) ); response ) {
		try {
			auto jsonValue  = nlohmann::json::parse( response.value() );
			auto memberJson = jsonValue.at( "Member" );
			d->mAddress		= memberJson.at( "Addr" ).get<std::string>();
		} catch( const nlohmann::json::parse_error & e ) {
			spdlog::error( "consulcpp::Consul::connect parser error: {}. Response was: {}", e.what(), response.value() );
		} catch( const std::exception & e ) {
			spdlog::error( "consulcpp::Consul::connect error: {}. Response was: {}", e.what(), response.value() );
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

const consulcpp::Services & consulcpp::Consul::services() const
{
	return d->mServices;
}

const consulcpp::Catalog & consulcpp::Consul::catalog() const
{
	return d->mCatalog;
}

const consulcpp::Sessions & consulcpp::Consul::sessions() const
{
	return d->mSessions;
}

const consulcpp::Leader & consulcpp::Consul::leader() const
{
	return d->mLeader;
}

const consulcpp::KV & consulcpp::Consul::kv() const
{
	return d->mKV;
}
