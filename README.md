# consulcpp

A C++ library that implements the Consul API.

A this moment only some endpoints and actions are supported:

- Register and deregister services with HTTP or gPRC health checks
- Set, get and delete key/values
- Create and delete sessions
- Leader election using sessions

Example:

```c++
#include <consulcpp/ConsulCpp>

int main( int argc, char * argv[] )
{
    consulcpp::Consul   consul;

    if( consul.connect() ){
        consulcpp::Service        service;
        consulcpp::ServiceCheck   check;

        service.mName = "demo";
        service.mAddress = consul.address();
        service.mPort = 50051;

        check.mInterval = "10s";
        check.mGRPC = fmt::format( "{}:{}/Health", service.mAddress, service.mPort );
        service.mChecks = { check };

        consul.services().create( service );

        consulcpp::Session     session = consul.sessions().create();

        consulcpp::Leader::Status leader = consul.leader().acquire( service, session );
        if( leader == consulcpp::Leader::Status::Yes ){
            // I'm the leader
        }else{
            // I'm a follower
        }
        ...
        consul.leader().release( service, session );
        consul.sessions().destroy( session );
        consul.services().destroy( service );
    }
    return 0;
}
```

The library uses [beast](https://www.boost.org/doc/libs/develop/libs/beast/doc/html/index.html) internally to access the Consul API. For now the use is inefficient.

## Building

Use cmake to build it on Windows, macOS and Linux.

### Dependencies using VCPKG

```
./vcpkg install boost
./vcpkg install fmt
./vcpkg install uriparser
./vcpkg install nlohmann-json
./vcpkg install spdlog
./vcpkg install protobuf
./vcpkg install grpc
```

## Use with CMake

```CMake
find_package(consulcpp CONFIG REQUIRED)
target_link_libraries(main PRIVATE consulcpp)
```

## Other Consul Clients in C++

- [oatpp-consul](https://github.com/oatpp/oatpp-consul): C++ Consul integration for oatpp applications
- [Ppconsul](https://github.com/oliora/ppconsul): C++ client for Consul
