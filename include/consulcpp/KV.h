#pragma once

#include <string>
#include <memory>

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

	/*! Returns the value of a single key
	*/
	stdx::optional<std::string> get( const std::string & key ) const;
	/*! Sets the value of a key, creates it if required
		Return true on success.
	*/
	bool set( const std::string & key, const std::string & value ) const;
	/*! Deletes the key/value.
		Returns true on success.
	*/
	bool destroy( const std::string & key ) const;

private:
	struct Private;
	std::unique_ptr<Private>	d;
};

}
