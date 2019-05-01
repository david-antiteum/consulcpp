#pragma once

#include <string>
#include <optional>

#include "consulcpp/Export.h"

namespace consulcpp {

class Consul;

/*! Implements KV Store Endpoints https://www.consul.io/api/kv.html
*/
class ConsulCPP_API KV
{
public:
	KV( Consul & consul );
	~KV();

	/*! Return the value of a single key
	*/
	std::optional<std::string> get( const std::string & key ) const;
	/*! Set the value of a key, creating it if required
		Return true on success.
	*/
	bool set( const std::string & key, const std::string & value ) const;
	/*! Delete the key/value.
		Return true on success.
	*/
	bool destroy( const std::string & key ) const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
