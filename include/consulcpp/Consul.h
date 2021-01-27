#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"
#include "consulcpp/spimpl.h"

namespace consulcpp {

class Services;
class Catalog;
class Sessions;
class Leader;
class KV;

/*! Consul services and agent data.

	This class has information about the local agent and access to consul services. 
*/
class ConsulCPP_API Consul
{
public:
	//! Creates a consul services object
	Consul();	

	/*! Connects to an agent
	*/
	[[nodiscard]] bool connect( std::string_view agentAddress = "http://127.0.0.1:8500" );

	//! Address where consul agent is running	
	[[nodiscard]] std::string address() const;
	//! Agent end point address, including the schema and port
	[[nodiscard]] std::string agentAddress() const;
	//! Consul API Version
	[[nodiscard]] std::string agentAPIVersion() const;

	// Services access
	[[nodiscard]] const Services & services() const;
	// Catalog access
	[[nodiscard]] const Catalog & catalog() const;
	// Sessions access
	[[nodiscard]] const Sessions & sessions() const;
	// Leader election access
	[[nodiscard]] const Leader & leader() const;
	// KV Storer access
	[[nodiscard]] const KV & kv() const;

private:
	struct Private;
	spimpl::impl_ptr<Private>	d;
};

}
