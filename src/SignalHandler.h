#ifndef __SIGNALHANDLER_H__
#define __SIGNALHANDLER_H__

#include "server/Server.h"

bool bindSignalHandler(HttpServer::Server *server) noexcept;

void stopSignalHandlers() noexcept;

#endif
