#ifndef __SERVERHTTP2SREAM_H__
#define __SERVERHTTP2SREAM_H__

#include "ServerHttp2Protocol.h"
#include "../../transfer/http2/Http2.h"
#include "../ServerControls.h"
#include "../ServerSettings.h"

namespace HttpServer
{
		class ServerHttp2Stream : public ServerHttp2Protocol						
		{
		public:
			ServerHttp2Stream(
											Socket::Adapter &sock,
											const ServerSettings &settings,
											ServerControls &controls,
											Http2::IncStream *stream
											) noexcept;

			virtual ServerProtocol *process() override;
			virtual void close() override;
			
		};
}

#endif
