
#include "consulcpp/KV.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

#include <boost/beast/core/detail/base64.hpp>
#include <gsl/gsl>

struct consulcpp::KV::Private
{
	consulcpp::Consul & mConsul;

	explicit Private( Consul & consul )
		: mConsul( consul )
	{
	}

	std::string api( std::string_view key ) const
	{
		return fmt::format( "{}/{}/kv/{}", mConsul.agentAddress(), mConsul.agentAPIVersion(), key );
	}
};

consulcpp::KV::KV( Consul & consul )
	: d( std::make_unique<Private>( consul ) )
{
}

consulcpp::KV::~KV() = default;

std::optional<std::string> consulcpp::KV::get( std::string_view key ) const
{
	std::optional<std::string> res;

	if( auto response = internal::HttpClient::get( d->api( key ) ); response && !response.value().empty() ) {
		spdlog::debug( "GET VALUE {}", response.value() );

		try {
			if( auto jsonValue = nlohmann::json::parse( response.value() ); jsonValue.is_array() ) {
				std::string value;
					value = jsonValue[ 0 ].at( "Value" ).get<std::string>();
				if( !value.empty() ) {
					gsl::owner<char *> out	= new char[ value.size() ];
					auto			   info = boost::beast::detail::base64::decode( out, value.c_str(), value.size() );
					res						= std::string( out, info.first );
				}
			}
		} catch( const std::exception & e ) {
			spdlog::error( "consulcpp::KV::get error: {}. Response was: {}", e.what(), response.value() );
		}
	}
	return res;
}

bool consulcpp::KV::set( std::string_view key, std::string_view value ) const
{
	bool res = false;

	if( auto response = internal::HttpClient::put( d->api( key ), value ); response ) {
		if( response.value().find( "true" ) != std::string::npos ) {
			res = true;
		}
	} else {
		spdlog::error( "KV set: Consul returns the error {}", response.error() );
	}
	return res;
}

bool consulcpp::KV::destroy( std::string_view key ) const
{
	bool res = false;

	if( auto response = internal::HttpClient::delete_( d->api( key ) ); response ) {
		if( response.value().find( "true" ) != std::string::npos ) {
			res = true;
		}
	} else {
		spdlog::error( "KV delete: Consul returns the error {}", response.error() );
	}
	return res;
}
