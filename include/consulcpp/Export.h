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
