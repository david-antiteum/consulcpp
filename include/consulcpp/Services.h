#pragma once

#include <string>
#include <memory>
#include <optional>
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

	/*! Register a new service.
	*/
	void create( const Service & service );
	/*! Deregister a service.
	*/
	void destroy( const Service & id );

	/*! Look for a service in the local agent
	*/
	std::optional<Service> findInLocal( const std::string & id ) const;
	/*! Look for a service in the catalog
	*/
	std::vector<Service> findInCatalog( const std::string & name, const std::vector<std::string> & tags ) const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
