#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

#include "consulcpp/Export.h"
#include "consulcpp/spimpl.h"

namespace consulcpp {

class Consul;
struct Service;

/*! Implements Agent Service Endpoints https://www.consul.io/api/agent/service.html
*/
class ConsulCPP_API Catalog
{
public:
	explicit Catalog( Consul & consul );

	/*! Looks for a service in the catalog
	*/
	std::vector<Service> find( std::string_view name, const std::vector<std::string> & tags = {} ) const;

private:
	struct Private;
	spimpl::impl_ptr<Private>	d;
};

}
