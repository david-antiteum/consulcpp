
#include <fmt/format.h>
#include "consulcpp/Service.h"

namespace consulcpp {

void to_json( nlohmann::json & j, const ServiceCheck & s ) 
{
	j = nlohmann::json{
		{"Id", s.mId}, {"Name", s.mName}, {"Interval", s.mInterval},
		{"Notes", s.mNotes}, {"ServiceID", s.mServiceID}, {"Status", s.mStatus},
		{"Method", s.mMethod}, {"Headers", s.mHeaders}
	};
	if( !s.mHTTP.empty() ){
		j["HTTP"] = s.mHTTP;
	}
	if( !s.mGRPC.empty() ){
		j["GRPC"] = s.mGRPC;
	}
}

void from_json( const nlohmann::json & j, ServiceCheck & s )
{
}

//---

void to_json( nlohmann::json & j, const Service & s ) 
{
	j = nlohmann::json{
		{ "Id", s.mId }, { "Name", s.mName }, { "Address", s.mAddress }, { "Port", s.mPort },
		{ "Kind", s.mKind }, { "EnableTagOverride", s.mEnableTagOverride },
		{ "Tags", s.mTags }, { "Meta", s.mMeta }
	};
	if( s.mChecks.size() == 1){
		j[ "Check" ] = s.mChecks.front();
	}else if( s.mChecks.size() > 1 ){
		j[ "Checks" ] = s.mChecks;
	}
}

void from_json( const nlohmann::json & j, Service & p )
{
}

}
