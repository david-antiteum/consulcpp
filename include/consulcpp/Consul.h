#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"

namespace consulcpp {

class Services;
class Sessions;
class Leader;

class ConsulCPP_API Consul
{
public:
	Consul();
	Consul( const std::string & agentAddress );	
	~Consul();

	bool connect();

	//! Address where consul agent is running	
	std::string address() const;
	//! Agent end point address, including the schema and port
	std::string agentAddress() const;
	//! Consul API Version
	std::string agentAPIVersion() const;

	Services & services() const;
	Sessions & sessions() const;
	Leader & leader() const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
