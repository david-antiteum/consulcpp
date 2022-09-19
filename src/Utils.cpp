#include <consulcpp/Utils.h>

#include <spdlog/spdlog.h>

namespace consulcpp::utils {

	[[nodiscard]] std::optional<unsigned short> asPort( const std::string & portText )
	{
		std::optional<unsigned short>	res;

		try{
			if( int resMaybe = std::stoi( portText ); resMaybe >= 0 && resMaybe <= std::numeric_limits<unsigned short>::max() ){
				res = static_cast<unsigned short>( resMaybe );
			}else{
				spdlog::error( "Port: Out of range" );
			}
		}catch( const std::invalid_argument & ia ){
			spdlog::error( "Port: Invalid argument {}", ia.what() );
		}catch( const std::out_of_range & oor ){
			spdlog::error( "Port: Out of range {}", oor.what() );
		}
		return res;
	}

}