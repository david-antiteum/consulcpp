#pragma once

#include <csignal>
#include <chrono>
#include <thread>

#if defined(_MSC_VER)
#define DISABLE_WARNING_PUSH           __pragma(warning( push ))
#define DISABLE_WARNING_POP            __pragma(warning( pop ))
#define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(4100)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(4505)
#define DISABLE_WARNING_DLL_INTERFACE					 DISABLE_WARNING(4251)
#define DISABLE_WARNING_LOSS_OF_DATA					 DISABLE_WARNING(4244)
// other warnings you want to deactivate...
#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(-Wunused-parameter)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(-Wunused-function)
#define DISABLE_WARNING_DLL_INTERFACE
#define DISABLE_WARNING_LOSS_OF_DATA
// other warnings you want to deactivate...
#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define DISABLE_WARNING_UNREFERENCED_FUNCTION
#define DISABLE_WARNING_DLL_INTERFACE
#define DISABLE_WARNING_LOSS_OF_DATA
// other warnings you want to deactivate...
#endif

static std::sig_atomic_t gSignalStatus;

static void signalHandler( int signal )
{
	gSignalStatus = signal;
}

static void loop()
{
	using namespace std::chrono_literals;

	auto previousSignal = std::signal( SIGINT, ::signalHandler );
	while( gSignalStatus == 0 ){
		std::this_thread::sleep_for( 500ms );
	}
	std::signal( SIGINT, previousSignal );
}
