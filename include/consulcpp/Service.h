#pragma once

#include <string>
#include <vector>
#include <map>

#include <nlohmann/json.hpp>

#include "consulcpp/Export.h"

namespace consulcpp {

/*! A Service check. https://www.consul.io/docs/agent/checks.html
*/
struct ConsulCPP_API ServiceCheck
{
	std::string		mId;
	std::string		mName;
	std::string		mInterval;
	std::string		mNotes;
	std::string		mServiceID;
	std::string		mStatus;

	std::string		mDockerContainerID;

	std::string		mDeregisterCriticalServiceAfter;
	std::vector<std::string>			mArgs;

	std::string		mTTL;
	std::string		mGRPC;
	bool			mGRPCUseTLS = false;

	std::string		mHTTP;
	std::string		mMethod;
	std::map<std::string,std::vector<std::string>, std::less<>>	mHeaders;

	std::string		mTCP;

	std::string		mTimeout;
	bool			mTLSSkipVerify = false;
};

extern ConsulCPP_API void to_json( nlohmann::json & j, const ServiceCheck & s );
extern ConsulCPP_API void from_json( const nlohmann::json & j, ServiceCheck & s );

/*! A service in consul. https://www.consul.io/docs/agent/services.html

	Fields in this struct are used in three different scenarios:
	- to register the service
	- after finding it in the local agent (endpoint: /catalog/service/:service)
	- after finding it in find in the catalog (endpoint: /agent/service/:service)

	The JSON returned differs en each scenario. We will return an unified version.
	For example, mName correspont to:
	- Name: when registering the service
	- Service: when reading from the local agent
	- ServiceName: when reading from the catalog
*/
struct ConsulCPP_API Service
{
	Service() = default;
	Service( const Service & a ) = default;
	Service( Service && a) noexcept = default;
	~Service() = default;
	Service & operator=( const Service & s ) = default;
    Service & operator=( Service && a) noexcept = default;

	// In Consul an empty id is valid and the name is used instead
	[[nodiscard]] const std::string & id() const
	{
		if( !mId.empty() ){
			return mId;
		}else{
			return mName;
		}
	}

	std::string		mId;
	std::string		mName;
	std::string		mAddress;
	unsigned short	mPort = 0;
	std::string		mKind;
	bool			mEnableTagOverride = false;

	std::vector<std::string>			mTags;
	std::map<std::string,std::string, std::less<>>	mMeta;
	
	// Catalog data:
	std::string							mDatacenter;
	std::string							mNodeAddress;
	std::string							mNode;
	std::map<std::string,std::string, std::less<>>	mNodeMeta;
	int									mCreateIndex = 0;
	int									mModifyIndex = 0;

	// Data at creation time only:
	// Checks
	std::vector<ServiceCheck>			mChecks;
};

extern ConsulCPP_API void to_json( nlohmann::json & j, const Service & s );
extern ConsulCPP_API void from_json( const nlohmann::json & j, Service & s );

}
