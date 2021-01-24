#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"

namespace consulcpp {

class Services;
class Sessions;
class Leader;
class KV;

/*! Consul services and agent data.

	This class has information about the local agent and access to consul services. 
*/
class ConsulCPP_API Consul
{
public:
	//! Creates a consul services object pointing to a local agent at http://127.0.0.1:8500
	Consul();
	//! Creates a consul services object pointing to a local agent at "agentAddress"
	explicit Consul( std::string_view agentAddress );	
	~Consul();

	/*! Connects to the local agent
	*/
	[[nodiscard]] bool connect();

	//! Address where consul agent is running	
	[[nodiscard]] std::string address() const;
	//! Agent end point address, including the schema and port
	[[nodiscard]] std::string agentAddress() const;
	//! Consul API Version
	[[nodiscard]] std::string agentAPIVersion() const;

	// Services access
	[[nodiscard]] Services & services() const;
	// Sessions access
	[[nodiscard]] Sessions & sessions() const;
	// Leader election access
	[[nodiscard]] Leader & leader() const;
	// KV Storer access
	[[nodiscard]] KV & kv() const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
