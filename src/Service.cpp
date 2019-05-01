
#include <fmt/format.h>
#include "consulcpp/Service.h"

namespace consulcpp {

void to_json( nlohmann::json & j, const ServiceCheck & s ) 
{
	j = nlohmann::json{
		{"ID", s.mId}, {"Name", s.mName}, {"Interval", s.mInterval},
		{"Notes", s.mNotes}, {"ServiceID", s.mServiceID}, {"Status", s.mStatus},
		{"Method", s.mMethod}, {"Headers", s.mHeaders}
	};
	if( !s.mTTL.empty() ){
		j["TTL"] = s.mTTL;
	}
	if( !s.mHTTP.empty() ){
		j["HTTP"] = s.mHTTP;
	}
	if( !s.mGRPC.empty() ){
		j["GRPC"] = s.mGRPC;
	}
}

void from_json( const nlohmann::json & j, ServiceCheck & s )
{
	s.mId = j.value( "ID", std::string() );
	s.mName = j.value("Name", std::string() );
	s.mInterval = j.value("Interval", std::string() );
	s.mNotes = j.value("Notes", std::string() );
	s.mServiceID = j.value("ServiceID", std::string() );
	s.mStatus = j.value("Status", std::string() );
	s.mMethod = j.value("Method", std::string() );
	if( j.contains( "Headers" ) ){
		j.at("Headers" ).get_to( s.mHeaders );
	}
	s.mTTL = j.value("TTL", std::string() );
	s.mHTTP = j.value("HTTP", std::string() );
	s.mGRPC = j.value("GRPC", std::string() );
}

//---

void to_json( nlohmann::json & j, const Service & s ) 
{
	j = nlohmann::json{
		{ "ID", s.mId }, { "Name", s.mName }, { "Address", s.mAddress }, { "Port", s.mPort },
		{ "Kind", s.mKind }, { "EnableTagOverride", s.mEnableTagOverride },
		{ "Tags", s.mTags }, { "Meta", s.mMeta }
	};
	if( s.mChecks.size() == 1){
		j[ "Check" ] = s.mChecks.front();
	}else if( s.mChecks.size() > 1 ){
		j[ "Checks" ] = s.mChecks;
	}
}

// {"Address":"127.0.0.1","ContentHash":"1ddd2e08001df7ae","EnableTagOverride":false,"ID":"1556702600904","Meta":{},"Port":50051,"Service":"leader-demo","Tags":null,"Weights":{"Passing":1,"Warning":1}}
void from_json( const nlohmann::json & j, Service & s )
{
	// Catalog ot Agent?
	if( j.contains( "Datacenter") ){
		s.mId = j.value("ServiceID", std::string() );
		s.mName = j.value("ServiceName", std::string() );
		s.mAddress = j.value("ServiceAddress", std::string() );
		s.mPort = j.value("ServicePort", 0 );
		s.mKind = j.value("ServiceKind", std::string() );
		s.mEnableTagOverride = j.value("ServiceEnableTagOverride", false );
		if( j.contains( "ServiceTags" ) ){
			auto node = j.at("ServiceTags" );
			if( !node.empty() ){
				node.get_to( s.mTags );
			}
		}
		if( j.contains( "ServiceMeta" ) ){
			j.at("ServiceMeta" ).get_to( s.mMeta );
		}

		s.mDatacenter = j.value("Datacenter", std::string() );
		s.mNodeAddress = j.value("Address", std::string() );
		s.mNode = j.value("Node", std::string() );

		if( j.contains( "NodeMeta" ) ){
			j.at("NodeMeta" ).get_to( s.mNodeMeta );
		}
	
		s.mCreateIndex = j.value("CreateIndex", 0 );
		s.mModifyIndex = j.value("ModifyIndex", 0 );		
	}else{
		s.mId = j.value("ID", std::string() );
		s.mName = j.value("Service", std::string() );
		s.mAddress = j.value("Address", std::string() );
		s.mPort = j.value("Port", 0 );
		s.mKind = j.value("Kind", std::string() );
		s.mEnableTagOverride = j.value("EnableTagOverride", false );
		if( j.contains( "Tags" ) ){
			auto node = j.at("Tags" );
			if( !node.empty() ){
				node.get_to( s.mTags );
			}
		}
		if( j.contains( "Meta" ) ){
			j.at("Meta" ).get_to( s.mMeta );
		}
	}
}

}
