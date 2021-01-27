#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"
#include "consulcpp/spimpl.h"

namespace consulcpp {

class Consul;

struct Session
{
	std::string		mId;
};

/*! Implements Session Endpoints https://www.consul.io/api/session.html
*/
class ConsulCPP_API Sessions
{
public:
	explicit Sessions( Consul & consul );

	/*! Creates a new session
	*/
	Session create() const;
	/*! Deletes the session.
		Returns true on success.
	*/
	bool destroy( const Session & session ) const;

private:
	struct Private;
	spimpl::impl_ptr<Private>	d;
};

}
