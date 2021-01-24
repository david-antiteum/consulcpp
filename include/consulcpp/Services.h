#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

#include "consulcpp/Export.h"

namespace consulcpp {

class Consul;
struct Service;

/*! Implements Agent Service Endpoints https://www.consul.io/api/agent/service.html
*/
class ConsulCPP_API Services
{
public:
	explicit Services( Consul & consul );
	~Services();

	/*! Registers a new service.
	*/
	void create( const Service & service ) const;
	/*! Deregisters a service.
	*/
	void destroy( const Service & service) const;

	/*! Looks for a service in the local agent
	*/
	std::optional<Service> findInLocal( std::string_view id ) const;
	/*! Looks for a service in the catalog
	*/
	std::vector<Service> findInCatalog( std::string_view name, const std::vector<std::string> & tags ) const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
