#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"

namespace consulcpp {

class Consul;
struct Service;
struct Session;

class ConsulCPP_API Leader
{
public:
	Leader( Consul & consul );
	~Leader();

	enum class Status {
		Yes,
		No,
		Error
	};

	Status acquire( const Service & service, const Session & session ) const;
	void release( const Service & service, const Session & session );

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
