
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

void consulcpp::Services::create( const Service & service )
{
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.put( fmt::format( "{}/{}/agent/service/register", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion() ), service );
}

void consulcpp::Services::destroy( const Service & service )
{
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.put( fmt::format( "{}/{}/agent/service/deregister/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), service.id() ), {} );
}

std::optional<consulcpp::Service> consulcpp::Services::findInLocal( const std::string & id ) const
{
	std::optional<consulcpp::Service>	res;
	consulcpp::internal::HttpClient		restClient;

	auto jsonMaybe = restClient.get( fmt::format( "{}/{}/agent/service/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), id ) );
	if( jsonMaybe ){
		nlohmann::json	json = jsonMaybe.value();
		res = json.get<consulcpp::Service>();
	}
	return res;
}

std::vector<consulcpp::Service> consulcpp::Services::findInCatalog( const std::string & name, const std::vector<std::string> & tags ) const
{
	std::vector<consulcpp::Service>		res;
	consulcpp::internal::HttpClient		restClient;
	std::string							path = fmt::format( "{}/{}/catalog/service/{}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), name );
	std::string							query;

	if( !tags.empty() ){
		for( const std::string & tag: tags ){
			if( query.empty() ){
				query += fmt::format( "tag={}", tag );
			}else{
				query += fmt::format( "&tag={}", tag );
			}
		}
	}
	if( !query.empty() ){
		path += "?" + query;
	}
	auto jsonMaybe = restClient.get( path );
	if( jsonMaybe && jsonMaybe.value().is_array() ){
		for( auto val: jsonMaybe.value() ){
			res.push_back( val.get<consulcpp::Service>() );
		}
	}
	return res;
}
