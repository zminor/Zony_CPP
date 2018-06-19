#include "SignalHandler.h"
#include "system/System.h"

#include <csignal>

static HttpServer::Server * globalServerPtr = nullptr;
/**
 * Terminate signal
 */
static void handlerSigTerm(const int) noexcept
{
	if(globalServerPtr)
	{
		globalServerPtr->stop();
	}
}
/**
 *	Interrupt signal
 */
static void handlerSigInt(const int) noexcept
{
	if(globalServerPtr)
	{
		globalServerPtr->stop();
	}
}
/**
 *	Signal to restart
 */
static void handlerSigUsr1(const int) noexcept
{
	if(globalServerPtr)
	{
		globalServerPtr -> restart();
	}
}
/**
 *	Signal to update modules
 */
static void handlerSigUsr2(const int) noexcept
{
	if(globalServerPtr)
	{
		globalServerPtr->update();
	}
}

bool bindSignalHandlers(HttpServer::Server *server) noexcept
{
	globalServerPtr = server;

	struct ::sigaction act{};						//Struct sigaction

	act.sa_handler = handlerSigInt; 		//Interrupt
	::sigaction(SIGINT, &act, nullptr);

	act.sa_handler = handlerSigTerm;		//Terminate
	::sigaction(SIGTERM, &act, nullptr);

	act.sa_handler = handlerSigUsr1;		//Restart
	::sigaction(SIGUSR1, &act, nullptr);

	act.sa_handler = handlerSigUsr2;		//Update Module
	::sigaction(SIGUSR2, &act, nullptr);

	::signal(SIGPIPE, SIG_IGN);					//Ignore to avoid program quit
	return true;
}

void stopSignalHander() noexcept
{
	
}
