#pragma once

#include <string>
#include <memory>
#include <vector>

#include "consulcpp/Export.h"

namespace consulcpp {

class Consul;
struct Service;

/*! Implements Agent Service Endpoints https://www.consul.io/api/agent/service.html
*/
class ConsulCPP_API Services
{
public:
	Services( Consul & consul );
	~Services();

	/*! Registers a new service.
	*/
	void create( const Service & service );
	/*! Deregisters a service.
	*/
	void destroy( const Service & id );

	/*! Looks for a service in the local agent
	*/
	stdx::optional<Service> findInLocal( const std::string & id ) const;
	/*! Looks for a service in the catalog
	*/
	std::vector<Service> findInCatalog( const std::string & name, const std::vector<std::string> & tags ) const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
