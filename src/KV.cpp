
#include "consulcpp/KV.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

#include <boost/beast/core/detail/base64.hpp>

struct consulcpp::KV::Private
{
	consulcpp::Consul		& mConsul;

	Private( Consul & consul )
		: mConsul( consul )
	{
	}
};

consulcpp::KV::KV( Consul & consul ) : d( std::make_unique<Private>( consul ) )
{

}

consulcpp::KV::~KV()
{
}

std::optional<std::string> consulcpp::KV::get( const std::string & key ) const
{
	std::optional<std::string>			res;
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.get( fmt::format( "{}/{}/kv/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), key ) );
	if( jsonMaybe ){
		if( jsonMaybe.value().is_array() ){
			auto 			jsonValue = jsonMaybe.value();
			std::string 	value;
			try{
				value = jsonValue[0].at( "Value" ).get<std::string>();
			}catch( const std::exception & e ){
				spdlog::error( "{}. Json was: {}", e.what(), jsonValue.dump() );
			}
			if( !value.empty() ){
				res = boost::beast::detail::base64_decode( value );
			}
		}
	}
	return res;
}

bool consulcpp::KV::set( const std::string & key, const std::string & value ) const
{
	consulcpp::internal::HttpClient		restClient;
	bool								res = false;

	auto response = restClient.putAsString( fmt::format( "{}/{}/kv/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), key ), value );
	if( response ){
		if( response.value().find( "true" ) != std::string::npos ){
			res = true;
		}
	}else{
		spdlog::error( "KV set: Consul returns the error {}", response.error() );
	}
	return res;
}

bool consulcpp::KV::destroy( const std::string & key ) const
{
	consulcpp::internal::HttpClient		restClient;
	bool								res = false;

	auto response = restClient.deleteAsString( fmt::format( "{}/{}/kv/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), key ) );
	if( response ){
		if( response.value().find( "true" ) != std::string::npos ){
			res = true;
		}
	}else{
		spdlog::error( "KV delete: Consul returns the error {}", response.error() );
	}
	return res;
}
