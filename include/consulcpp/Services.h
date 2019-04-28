#pragma once

#include <string>
#include <memory>

#include "consulcpp/Export.h"

namespace consulcpp {

class Consul;
struct Service;

class ConsulCPP_API Services
{
public:
	Services( Consul & consul );
	~Services();

	void publish( const Service & service );
	void unpublish( const Service & id );

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
