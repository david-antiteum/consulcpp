#pragma once

#ifdef ConsulCPP_DLL
	#ifdef WIN32
		#pragma warning(disable:4251)
		#ifdef ConsulCPP_EXPORTS 
			#define ConsulCPP_API __declspec(dllexport)
		#else
			#define ConsulCPP_API __declspec(dllimport)
		#endif
	#else
		#ifdef ConsulCPP_EXPORTS
			#define ConsulCPP_API __attribute__((visibility("default")))
		#else
			#define ConsulCPP_API
		#endif
	#endif
#else
	#define ConsulCPP_API
#endif

// Temporary hack for Ubuntu 16 default gcc compiler
// https://codereview.stackexchange.com/a/136373
#if __has_include(<optional>)
#include <optional>
namespace stdx {
	using namespace ::std;
}
#elif __has_include(<experimental/optional>)
#include <experimental/optional>
namespace stdx {
	using namespace ::std;
	using namespace ::std::experimental;
}
#endif
