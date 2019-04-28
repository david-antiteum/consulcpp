#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"

namespace consulcpp {

class Consul;

struct Session
{
	std::string		mId;
};

class ConsulCPP_API Sessions
{
public:
	Sessions( Consul & consul );
	~Sessions();

	Session create() const;
	void destroy( const Session & session );

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
