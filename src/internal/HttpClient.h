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
#include <tl/expected.hpp>

using tcp = boost::asio::ip::tcp; // from <boost/asio.hpp>
namespace http = boost::beast::http; // from <beast/http.hpp>

namespace fmt {
	template <>
	struct formatter<http::status> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const http::status & s, FormatContext &ctx) const {
			return format_to(ctx.out(), "{}", static_cast<int>( s ) );
		}
	};
}

namespace consulcpp::internal {

class HttpClient
{
public:
	[[nodiscard]] static tl::expected<std::string,http::status> get( std::string_view address )
	{
		return execute( address, {}, http::verb::get );
	}

	[[nodiscard]] static tl::expected<std::string,http::status> post( std::string_view address, std::string_view value )
	{
		return execute( address, value, http::verb::post );
	}

	[[nodiscard]] static tl::expected<std::string,http::status> post( std::string_view address, const nlohmann::json & value )
	{
		return execute( address, value.dump(), http::verb::post );
	}

	[[nodiscard]] static tl::expected<std::string,http::status> put( std::string_view address, std::string_view value = "" )
	{
		return execute( address, value, http::verb::put );
	}

	[[nodiscard]] static tl::expected<std::string,http::status> put( std::string_view address, const nlohmann::json & value )
	{
		return execute( address, value.dump(), http::verb::put );
	}

	[[nodiscard]] static tl::expected<std::string,http::status> delete_( std::string_view address )
	{
		return execute( address, {}, http::verb::delete_ );
	}

private:
	static tl::expected<std::string,http::status> execute( std::string_view address, std::string_view value, http::verb verb )
	{
		Uri uri( address );

		if( !uri.isValid() ){
			spdlog::error( "cannot parse uri {}", address );
			return tl::make_unexpected( http::status::bad_request );
		}
		// URI OK...
		boost::asio::io_context 		ios;
		tcp::socket 					sock{ ios };
		boost::beast::error_code 		ecConnect;

		tcp::endpoint endpoint( boost::asio::ip::make_address( uri.host() ), uri.port() );
		sock.connect( endpoint, ecConnect );
		if( ecConnect ){
			spdlog::error( "Error conecting to {}. {}", address, ecConnect.message() );
			return tl::make_unexpected( http::status::service_unavailable );
		}else{
			tl::expected<std::string,http::status>	res;

			if( auto ecWrite = write( sock, uri, verb, value ); ecWrite ){
				spdlog::error( "Error sending request to {}. {}", address, ecWrite.message() );
				res = tl::make_unexpected( http::status::service_unavailable );
			}else{
				// Write OK...
				res = read( sock, uri );
			}
			boost::beast::error_code 		ecShutdown;
			sock.shutdown( tcp::socket::shutdown_both, ecShutdown );

			return res;
		}
	}

	static boost::beast::error_code write( tcp::socket & sock, const Uri & uri, http::verb verb, std::string_view value )
	{
		http::request<http::string_body> 	req;

		req.method( verb );
		if( auto query = uri.query(); query.empty() ){
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

		boost::beast::error_code 			error;

		http::write( sock, req, error );

		return error;
	}

	static tl::expected<std::string,http::status> read( tcp::socket & sock, const Uri & uri )
	{
		tl::expected<std::string,http::status>	res;
		boost::beast::flat_buffer 				b;
		http::response<http::string_body> 		response;
		boost::beast::error_code 				error;

		http::read( sock, b, response, error );
		if( error ){
			spdlog::error( "Error reading from {}. {}", uri.asString(), error.message() );
			res = tl::make_unexpected( http::status::service_unavailable );
		}else{
			res = response.body();
		}
		return res;
	}
};

}
