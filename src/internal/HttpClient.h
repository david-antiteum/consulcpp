#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <optional>
#include <tuple>

#include <cstdlib>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "internal/URI.h"

using tcp = boost::asio::ip::tcp; // from <boost/asio.hpp>
namespace http = boost::beast::http; // from <beast/http.hpp>

namespace consulcpp { namespace internal {

class HttpClient
{
public:
	static std::optional<nlohmann::json> get( const std::string & address )
	{
		return executeJson( address, {}, http::verb::get );
	}

	static std::optional<nlohmann::json> post( const std::string & address, const nlohmann::json & json )
	{
		return executeJson( address, json, http::verb::post );
	}

	static std::optional<nlohmann::json> put( const std::string & address, const nlohmann::json & json )
	{
		return executeJson( address, json, http::verb::put );
	}

	static std::string putAsString( const std::string & address, const nlohmann::json & json )
	{
		return executeString( address, json, http::verb::put );
	}

private:
	static std::optional<nlohmann::json> executeJson( const std::string & address, const nlohmann::json & json, http::verb verb )
	{
		const auto response = execute( address, json.empty() ? "" : json.dump(), verb );

		if( std::get<1>( response ) == http::status::ok ){
			try{
				return nlohmann::json::parse( std::get<0>( response ) );
			}catch(...){
				// TODO report parsing error
			}
		}else{
			spdlog::error( "execute<json> error {}", static_cast<int>( std::get<1>( response )));
		}
		return {};
	}

	static std::string executeString( const std::string & address, const nlohmann::json & json, http::verb verb )
	{
		const auto response = execute( address, json.empty() ? "" : json.dump(), verb );

		if( std::get<1>( response ) == http::status::ok ){
			return std::get<0>( response );
		}else{
			spdlog::error( "execute<string> error {}", static_cast<int>( std::get<1>( response )));
		}
		return {};
	}

	static std::tuple<std::string,http::status> execute( const std::string & address, const std::string & value, http::verb verb )
	{
		std::tuple<std::string,http::status>	res{ "", http::status::internal_server_error };
		Uri 									uri( address );
		boost::beast::error_code 				ec;

		if( !uri.isValid() ){
			spdlog::error( "cannot parse uri {}", address );
			std::get<1>(res) = http::status::bad_request;
		}else{
			boost::asio::io_service ios;
			tcp::socket sock{ ios };

			tcp::endpoint endpoint( boost::asio::ip::address::from_string( uri.host() ), uri.port() );
			sock.connect( endpoint, ec );
			if( ec ){
				spdlog::error( "Error conecting to {}. {}", address, ec.message() );
				std::get<1>(res) = http::status::service_unavailable;
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

				http::write( sock, req, ec );
				if( ec ){
					spdlog::error( "Error sending request to {}. {}", address, ec.message() );
					std::get<1>(res) = http::status::service_unavailable;
				}else{
					boost::beast::flat_buffer b;

					http::response<http::string_body> response;
					http::read(sock, b, response, ec);
					if( ec ){
						spdlog::error( "Error reading from {}. {}", address, ec.message() );
					}else{
						res = { response.body(), response.result() };
					}
				}
				sock.shutdown(tcp::socket::shutdown_both, ec);
			}
		}
		return res;
	}
};

}}
