#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <tuple>

#include <cstdlib>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "Uri.h"
#include "expected.hpp"

using tcp = boost::asio::ip::tcp; // from <boost/asio.hpp>
namespace http = boost::beast::http; // from <beast/http.hpp>

namespace fmt {
	template <>
	struct formatter<http::status> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const http::status & s, FormatContext &ctx) {
			return format_to(ctx.out(), "{}", static_cast<int>( s ) );
		}
	};
}

namespace consulcpp { namespace internal {

class HttpClient
{
public:
	static tl::expected<std::string,http::status> get( const std::string & address )
	{
		return execute( address, {}, http::verb::get );
	}

	static tl::expected<std::string,http::status> post( const std::string & address, const std::string & value )
	{
		return execute( address, value, http::verb::post );
	}

	static tl::expected<std::string,http::status> post( const std::string & address, const nlohmann::json & value )
	{
		return execute( address, value.dump(), http::verb::post );
	}

	static tl::expected<std::string,http::status> put( const std::string & address, const std::string & value = "" )
	{
		return execute( address, value, http::verb::put );
	}

	static tl::expected<std::string,http::status> put( const std::string & address, const nlohmann::json & value )
	{
		return execute( address, value.dump(), http::verb::put );
	}

	static tl::expected<std::string,http::status> delete_( const std::string & address )
	{
		return execute( address, {}, http::verb::delete_ );
	}

private:
	static tl::expected<std::string,http::status> execute( const std::string & address, const std::string & value, http::verb verb )
	{
		tl::expected<std::string,http::status>	res;
		Uri 									uri( address );

		if( !uri.isValid() ){
			spdlog::error( "cannot parse uri {}", address );
			res = tl::make_unexpected( http::status::bad_request );
		}else{
			boost::asio::io_service 		ios;
			tcp::socket 					sock{ ios };
			boost::beast::error_code 		ec;

			tcp::endpoint endpoint( boost::asio::ip::address::from_string( uri.host() ), uri.port() );
			sock.connect( endpoint, ec );
			if( ec ){
				spdlog::error( "Error conecting to {}. {}", address, ec.message() );
				res = tl::make_unexpected( http::status::service_unavailable );
			}else{
				http::request<http::string_body> req;
				req.method( verb );

				auto query = uri.query();
				if( query.empty() ){
					req.target( uri.path() );
				}else{
					req.target( uri.path() + "?" + query );
				}
				req.set( http::field::host, uri.host() + ":" + std::to_string( uri.port() ) );
				req.set( http::field::user_agent, BOOST_BEAST_VERSION_STRING );
				if( !value.empty() ){
					req.body() = value;
				}
				req.prepare_payload();

				boost::beast::error_code 	ec;
				http::write( sock, req, ec );
				if( ec ){
					spdlog::error( "Error sending request to {}. {}", address, ec.message() );
					res = tl::make_unexpected( http::status::service_unavailable );
				}else{
					boost::beast::flat_buffer 			b;
					http::response<http::string_body> 	response;
					boost::beast::error_code 			ec;

					http::read( sock, b, response, ec );
					if( ec ){
						spdlog::error( "Error reading from {}. {}", address, ec.message() );
						res = tl::make_unexpected( http::status::service_unavailable );
					}else{
						res = response.body();
					}
				}
				sock.shutdown(tcp::socket::shutdown_both, ec);
			}
		}
		return res;
	}
};

}}
