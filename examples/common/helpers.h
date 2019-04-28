#pragma once

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
