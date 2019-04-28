#pragma once

#include <string>
#include <vector>
#include <map>

#include <nlohmann/json.hpp>

#include "consulcpp/Export.h"

namespace consulcpp {

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

	std::string		mGRPC;
	bool			mGRPCUseTLS = false;

	std::string		mHTTP;
	std::string		mMethod;
	std::map<std::string,std::vector<std::string>>	mHeaders;

	std::string		mTCP;

	std::string		mTimeout;
	bool			mTLSSkipVerify = false;
};

extern ConsulCPP_API void to_json( nlohmann::json & j, const ServiceCheck & s );
extern ConsulCPP_API void from_json( const nlohmann::json & j, ServiceCheck & s );

struct ConsulCPP_API Service
{
	std::string		mId;
	std::string		mName;
	std::string		mAddress;
	int				mPort = 0;
	std::string		mKind;
	bool			mEnableTagOverride = false;

	std::vector<std::string>			mTags;
	std::map<std::string,std::string>	mMeta;
	std::vector<ServiceCheck>			mChecks;
};

extern ConsulCPP_API void to_json( nlohmann::json & j, const Service & s );
extern ConsulCPP_API void from_json( const nlohmann::json & j, Service & s );

}
