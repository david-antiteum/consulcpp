#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"

namespace consulcpp {

class Consul;
struct Service;
struct Session;

/*! Implements leader election using sessions as documented at https://www.consul.io/docs/guides/leader-election.html
*/
class ConsulCPP_API Leader
{
public:
	Leader( Consul & consul );
	~Leader();

	/*! Leader status:
		- Yes: you are the leader
		- No: you are not the leader
		- Error: and error happens
	*/
	enum class Status {
		Yes,
		No,
		Error
	};

	/*! Tries to be the leader (using a service name and session id).
	*/
	Status acquire( const Service & service, const Session & session ) const;
	/*! Steps down as the leader (using a service name and session id).
	*/
	void release( const Service & service, const Session & session ) const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
