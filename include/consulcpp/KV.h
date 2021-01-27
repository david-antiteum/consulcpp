#pragma once

#include <string>
#include <memory>
#include <optional>

#include "consulcpp/Export.h"
#include "consulcpp/spimpl.h"

namespace consulcpp {

class Consul;

/*! Implements KV Store Endpoints https://www.consul.io/api/kv.html
*/
class ConsulCPP_API KV
{
public:
	explicit KV( Consul & consul );

	/*! Returns the value of a single key
	*/
	std::optional<std::string> get( std::string_view key ) const;
	/*! Sets the value of a key, creates it if required
		Return true on success.
	*/
	bool set( std::string_view key, std::string_view value ) const;
	/*! Deletes the key/value.
		Returns true on success.
	*/
	bool destroy( std::string_view key ) const;

private:
	struct Private;
	spimpl::impl_ptr<Private>	d;
};

}
