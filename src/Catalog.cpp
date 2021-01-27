
#include "consulcpp/Catalog.h"
#include "consulcpp/Service.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

struct consulcpp::Catalog::Private
{
	consulcpp::Consul & mConsul;

	explicit Private( Consul & consul )
		: mConsul( consul )
	{
	}

	std::string api() const
	{
		return fmt::format( "{}/{}/catalog/service", mConsul.agentAddress(), mConsul.agentAPIVersion() );
	}
};

consulcpp::Catalog::Catalog( Consul & consul )
	: d( spimpl::make_impl<Private>( consul ) )
{
}

std::vector<consulcpp::Service> consulcpp::Catalog::find( std::string_view name, const std::vector<std::string> & tags ) const
{
	std::vector<consulcpp::Service> res;
	std::string						path = fmt::format( "{}/{}", d->api(), name );
	std::string						query;

	if( !tags.empty() ) {
		for( const std::string & tag: tags ) {
			if( query.empty() ) {
				query += fmt::format( "tag={}", tag );
			} else {
				query += fmt::format( "&tag={}", tag );
			}
		}
	}
	if( !query.empty() ) {
		path += "?" + query;
	}
	if( auto response = consulcpp::internal::HttpClient::get( path ); response ) {
		try{
			if( auto jv = nlohmann::json::parse( response.value() ); jv.type() == nlohmann::json::value_t::array ){
				for( auto val: jv ) {
					res.push_back( val.get<consulcpp::Service>() );
				}
			}else if( jv.type() == nlohmann::json::value_t::object ){
				res.push_back( jv );
			}else{
				spdlog::error( "consulcpp::Catalog::find error: neither an object not an array. Response was: {}", response.value() );
			}
		} catch( const nlohmann::json::parse_error & e ) {
			spdlog::error( "consulcpp::Catalog::find parser error: {}. Response was: {}", e.what(), response.value() );
		} catch( const std::exception & e ) {
			spdlog::error( "consulcpp::Catalog::find error: {}. Response was: {}", e.what(), response.value() );
		}
	}
	return res;
}
