#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"
#include "consulcpp/spimpl.h"

namespace consulcpp {

class Consul;
struct Service;
struct Session;

/*! Implements leader election using sessions as documented at https://www.consul.io/docs/guides/leader-election.html
*/
class ConsulCPP_API Leader
{
public:
	explicit Leader( Consul & consul );

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

	/*! Destroy KV data created to execute acquire
	*/
	void destroy( const Service & service ) const;

private:
	struct Private;
	spimpl::impl_ptr<Private>	d;
};

}
