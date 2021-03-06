#include "Server.h"
#include "ServerStructuresArguments.h"
#include "../SignalHandler.h"
#include "SocketsQueue.h"

#include "config/ConfigParser.h"
#include "data-variant/FormUrlencoded.h"
#include "data-variant/MultipartFormData.h"
#include "data-variant/TextPlain.h"
#include "protocol/ServerHttp1.h"
#include "protocol/ServerHttp2.h"
#include "protocol/ServerHttp2Stream.h"

#include "../socket/List.h"
#include "../socket/AdapterDefault.h"
#include "../socket/AdapterTls.h"
#include "../system/GlobalMutex.h"
#include "../system/SharedMemory.h"
#include "../system/System.h"
#include "../utils/Utils.h"
#include <thread>

namespace HttpServer
{

	//---------------------Port------------------//
	static bool tlsInit(
			const ServerApplicationSettings &app,
			std::tuple<gnutls_certificate_credentials_t, gnutls_priority_t> &data
			)
	{
			::gnutls_certificate_credentials_t x509_cred;
			
			int ret = ::gnutls_certificate_allocate_credentials(&x509_cred);

			if (ret < 0)
			{
				std::cout << "Error [tls]: certificate credentials has not been allocated; code: " << ret << std::endl;
				return false;
			}
			
			if (app.chain_file.empty() == false)
			{
				ret = ::gnutls_certificate_set_x509_trust_file(
												x509_cred,
												app.chain_file.c_str(),
												GNUTLS_X509_FMT_PEM
												);
				
				if (ret < 0) {
					std::cout << "Warning [tls]: (CA) chain file has not been accepted; code: " << ret << std::endl;
				}
			}
			
			if (app.crl_file.empty() == false)
			{
				ret = ::gnutls_certificate_set_x509_crl_file(
								x509_cred,
								app.crl_file.c_str(),
								GNUTLS_X509_FMT_PEM
								);
				
				if (ret < 0)
				{
					std::cout << "Warning [tls]: (CLR) clr file has not been accepted; code: " << ret << std::endl;
				}
			}
			
			ret = ::gnutls_certificate_set_x509_key_file(
								x509_cred,
								app.cert_file.c_str(),
								app.key_file.c_str(),
								GNUTLS_X509_FMT_PEM
								);
			
			if (ret < 0)
			{
				std::cout << "Error [tls]: (CERT) cert file or/and (KEY) key file has not been accepted; code: " << ret << std::endl;
				return false;
			}
			
			if (app.stapling_file.empty() == false)
			{
				ret = ::gnutls_certificate_set_ocsp_status_request_file(
												x509_cred,
												app.stapling_file.c_str(),
												0
												);
				
				if (ret < 0)
				{
					std::cout << "Warning [tls]: (OCSP) stapling file has not been accepted; code: " << ret << std::endl;
				}
			}
			
			::gnutls_dh_params_t dh_params;
			
			::gnutls_dh_params_init(&dh_params);
			
			if (app.dh_file.empty() )
			{
				const unsigned int bits = ::gnutls_sec_param_to_pk_bits(
												GNUTLS_PK_DH, GNUTLS_SEC_PARAM_HIGH
												);
				
				ret = ::gnutls_dh_params_generate2(
												dh_params,
												bits
												);
			}
			else
			{
				std::ifstream dh_file(app.dh_file);
				
				if (dh_file)
				{
					const size_t max_file_size = 1024 * 1024;
					
					std::vector<char> buf(max_file_size);
					
					dh_file.read(
													buf.data(),
													std::streamsize(buf.size())
													);
					
					gnutls_datum_t datum{
							reinterpret_cast<unsigned char *>(buf.data() ),
							static_cast<unsigned int>(dh_file.gcount() )
					};

					ret = ::gnutls_dh_params_import_pkcs3(
													dh_params,
													&datum,
													GNUTLS_X509_FMT_PEM
													);
				}
				else
				{
					ret = -1;
					std::cout << "Error [tls]: DH params file has not been opened;" << std::endl;
				}
				
				dh_file.close();
			}
			
			if (ret < 0)
			{
				::gnutls_certificate_free_credentials(x509_cred);
				std::cout << "Error [tls]: DH params were not loaded; code: " << ret << std::endl;
				return false;
			}
			
			::gnutls_certificate_set_dh_params(x509_cred, dh_params);
			
			::gnutls_priority_t priority_cache;
			
			ret = ::gnutls_priority_init(&priority_cache, "NORMAL", nullptr);
			
			if (ret < 0)
			{
				::gnutls_certificate_free_credentials(x509_cred);
				std::cout << "Error [tls]: priority cache cannot be init; code: " << ret << std::endl;
				return false;
			}
			
			data = std::tuple<gnutls_certificate_credentials_t, gnutls_priority_t>{
								x509_cred,
								priority_cache
			};
			
			return true;
	}

	bool Server::tryBindPort(
			const int port,
			std::unordered_set <int> & ports
			)	
	{
			if (ports.cend() != ports.find(port) )
			{
					return false;
			}
			
			Socket::Socket sock;
			
			if (sock.open() == false)
			{
					std::cout << "Error: socket cannot be open; errno " << Socket::Socket::getLastError() << ";" << std::endl;
					return false;
			}
			
			if (sock.bind(port) == false)
			{
					std::cout << "Error: port " << port << " cannot be bind; errno " << Socket::Socket::getLastError() << ";" << std::endl;
					return false;
			}
			
			if (sock.listen() == false)
			{
					std::cout << "Error: socket " << port << " cannot be listen; errno " << Socket::Socket::getLastError() << ";" << std::endl;
					return false;
			}
			
			sock.nonblock(true);
			
			this->listeners.emplace_back(std::move(sock) );
			
			ports.emplace(port);
			
			return true;
	}

	void Server::initAppsPorts()
	{
		std::unordered_set<ServerApplicationSettings *> applications;
		this->settings.apps_tree.collectApplicationSettings(applications);
		
		std::unordered_set<int> ports;
		
		for (auto const &app : applications)
		{
			const std::unordered_set<int> &tls = app->tls_ports;
						
			if (tls.empty() == false)
			{
				std::tuple<gnutls_certificate_credentials_t, gnutls_priority_t> data;
								
				if (tlsInit(*app, data) )
				{
					for (const int port : tls)
					{
						if (this->tryBindPort(port, ports) )
						{
							this->tls_data.emplace(port, data);
						}
					}
				}
			}
						
			const std::unordered_set<int> &list = app->ports;
						
			for (const int port : list) 
			{
				this->tryBindPort(port, ports);
			}
		}
	}

//--------------------Main Func-------------------//

	static std::unique_ptr<ServerProtocol> getProtocolVariant(
		Socket::Adapter &sock,
		const ServerSettings &settings,
		ServerControls &controls,
		SocketsQueue &sockets,
		Http2::IncStream *stream)
	{
		std::unique_ptr<ServerProtocol> prot;
		//If request is HTTP/2 Stream
		if(stream)
		{
//			prot.reset( new ServerHttp2Stream(sock,settings, controls,stream));
//			return prot;
		}

		if(sock.get_tls_session() != nullptr)
		{
			::gnutls_datum_t datum;

			const int ret = ::gnutls_alpn_get_selected_protocol(sock.get_tls_session(),&datum);

			if(GNUTLS_E_SUCCESS == ret)
			{
				const std::string protocol(reinterpret_cast<char*> (datum.data), datum.size);

//				if("h2" == protocol)
//				{
//					prot.reset(new ServerHttp2(sock,settings,controls,sockets));
//				}
//				else 
				if("http/1.1" == protocol)
				{
					prot.reset(new ServerHttp1(sock,settings,controls));
				}
			}
			else if(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE == ret)
			{
				prot.reset(new ServerHttp1(sock, settings, controls));
			}
	
			return prot;
		}

		prot.reset( new ServerHttp1(sock, settings, controls) );

		return prot;
	}


	int Server::run()
	{
		if (this->init() == false)
		{
			return 0x10;
		}

		this->initAppsPorts();
		
		if (this->listeners.empty() )
		{
			std::cout << "Error: any socket was not open;" << std::endl;
			this->clear();
			return 0x20;
		}

		Socket::List sockets_list;
		
		sockets_list.create(this->listeners.size() );
		
		for (auto const &sock : this->listeners) {
			sockets_list.addSocket(sock);
		}
		
		std::cout << "Log: server started work;" << std::endl << std::endl;
		
		constexpr size_t queue_max_length = 1024;
		this->controls.eventNotFullQueue = new Utils::Event(true, true);
		this->controls.eventProcessQueue = new Utils::Event();
		this->controls.eventUpdateModule = new Utils::Event(false, true);
		
		SocketsQueue sockets;
		
		this->controls.setProcess();
		
		std::function<int(Server *, SocketsQueue &)> serverCycleQueue = std::mem_fn(&Server::cycleQueue);
		std::thread threadQueue(serverCycleQueue, this, std::ref(sockets) );
		
		std::vector<Socket::Socket> accept_sockets;
		
		//Cycle for receiving new connections
		do
		{
			if (sockets_list.accept(accept_sockets) )
			{
				sockets.lock();
					
				for (size_t i = 0; i < accept_sockets.size(); ++i)
				{
					const Socket::Socket &sock = accept_sockets[i];
					
					if (sock.is_open() )
					{
						sock.nonblock(true);
						
						sock.tcp_nodelay(true);
						
						sockets.emplace(
								std::tuple<Socket::Socket, Http2::IncStream *>
								{
										sock,
										nullptr
								}
							);						
						}
					}

					sockets.unlock();
				
					this->controls.eventProcessQueue->notify();
				
					if (sockets.size() >= queue_max_length)
					{
						this->controls.eventNotFullQueue->reset();
					}				
					accept_sockets.clear();				
					this->controls.eventNotFullQueue->wait();
				}
		}
		while (this->controls.process_flag || this->controls.eventUpdateModule->notified() );
		
		this->controls.eventProcessQueue->notify();
		
		threadQueue.join();
		
		sockets_list.destroy();
		
		if (this->listeners.empty() == false)
		{
			for (Socket::Socket &sock : this->listeners)
			{
				sock.close();
				
			}
			
			this->listeners.clear();
		}
		
		this->clear();
		
		std::cout << "Log: server work completed;" << std::endl;
		
		return EXIT_SUCCESS;		
	}

	int Server:: cycleQueue(SocketsQueue &sockets)
	{
		//Setup Thread Count
		auto const it_option = this->settings.global.find("thread_max_count ");
		size_t thread_max_count = 0;

		if(this->settings.global.cend() != it_option)
		{
			const std::string &option = it_option->second;
			thread_max_count = std::strtoull(option.c_str(),nullptr,10);
		}

		if( 0 == thread_max_count)
		{
			//If 0,Get from hardware Info
			thread_max_count = std::thread::hardware_concurrency();
			//Still 0, Set max thread Count 1
			if( 0 == thread_max_count)
			{
				thread_max_count = 1;
			}

			thread_max_count *= 2;
		}
		//Working Thread count Start with 0
		this->threads_working_count = 0;
		//
		Utils::Event eventThreadCycle(false,true);

		//Define Function Calling threadRequestCyle
		std::function <void (
				Server *, SocketsQueue&, Utils::Event &						
		)> serverThreadRequestCycle = std::mem_fn( &Server :: threadRequestCycle);

		std::vector<std::thread> active_threads;
		active_threads.reserve(thread_max_count);

		do
		{
			if(this->controls.eventUpdateModule -> notified())
			{
				updateModules();
			}

			//Cycle creation threads application requests
			do{
				while(
					this->threads_working_count == active_threads.size()&&
					active_threads.size()< thread_max_count &&
					sockets.empty() == false )
				{
					active_threads.emplace_back(
													serverThreadRequestCycle,
													this,
													std::ref(sockets),
													std::ref(eventThreadCycle)
													);
				}
				//activate working Threads
				size_t notify_count = active_threads.size() - this->threads_working_count;
				if(notify_count > sockets.size())
				{
					notify_count = sockets.size();
				}

				eventThreadCycle.notify(notify_count);
				this->controls.eventProcessQueue->wait();
			}
			while(this->controls.process_flag);

			if(active_threads.empty() == false)
			{
				for(auto &th : active_threads)
				{
					th.join();
				}
				active_threads.clear();
			}
			this->controls.eventNotFullQueue->notify();
		}
		while(this->controls.eventUpdateModule -> notified() );

		return 0;	
	}


	void Server::updateModules()
	{
		//Application settings List
		std::unordered_set <ServerApplicationSettings *> applications;
		//Get Full application settings List
		this->settings.apps_tree.collectApplicationSettings(applications);
		//store all updated module Index into updated
		std::unordered_set <size_t> updated;
		//Loop through all applicationSettings
		for(auto const &app : applications )
		{
			 //Each application is a module for the Server, each has a index
			 const size_t module_index = app->module_index;
			 //If module is not updated
			 if(updated.cend() == updated.find(module_index))
			 {
					
					if(app->server_module_update.empty() == false && app->server_module_update != app->server_module)
					{
			 				size_t module_size_new = 0;
							time_t module_time_new = 0;

						if(System::getFileSizeAndTimeGmt(app->server_module_update, &module_size_new,&module_time_new))
						{
							size_t module_size_cur =0;
							time_t module_time_cur =0;

							System::Module & module = this->modules[module_index];

							if(System::getFileSizeAndTimeGmt(app->server_module,&module_size_cur,&module_time_cur))
							{
								if(module_size_cur != module_size_new || module_time_cur < module_time_new)
								{
									this->updateModule(module,applications,module_index);
								}
							}
						}
				 }
						updated.emplace(module_index);
			 }
		}

		std::cout << "Notice : application modules have been updated.." << std::endl;
		this->controls.setProcess();
		this->controls.eventUpdateModule->reset();
	}

	bool Server::updateModule(
									System::Module &module,
									std::unordered_set<ServerApplicationSettings *> &applications,
									const size_t moduleIndex
									)
	{
		std::unordered_set <ServerApplicationSettings *> same;

		for(auto &app : applications)
		{
			if( app -> module_index == moduleIndex)
			{
				same.emplace (app);

				try{
					if(app->application_final)
					{
						const std::string root = app->root_dir;
						app->application_final(root.data());
					}
				}
				catch( std::exception &exc )
				{
					std::cout << "Warning: exception was thrown when application "<< app->server_module << "was finishes"<< exc.what() << std::endl;
				}

				//setup functions
				app->application_call = std::function < int (Transfer::app_request *, Transfer::app_response *)> ();
				app->application_clear = std::function <void (void *, size_t )>();
				app->application_init  = std::function <bool (const char* )>();
				app->application_final = std::function <void (const char* )>();
			}
		}

		module.close();
		auto const app = *(same.cbegin() );
		const std::string &module_name = app->server_module;

		//Hack for posix system -load new version shared library
		const size_t dir_pos = module_name.rfind('/');
		const size_t ext_pos = module_name.rfind('.');

		std::string module_name_temp;

		if(std::string::npos != ext_pos && (std::string ::npos == dir_pos|| dir_pos < ext_pos))
		{
			module_name_temp = module_name.substr(0,ext_pos)+ '-'+ Utils::getUniqueName()+ module_name.substr(ext_pos);
		}
		else 
		{
			module_name_temp = module_name + '-' + Utils::getUniqueName();
		}
		//Input File Stream
		std::ifstream src(app->server_module_update, std::ifstream ::binary);

		if(!src)
		{
			std::cout << "Error: file" << app->server_module_update<<" cannot be open ..."<<std::endl;
			return false;
		}
		//Output File Stream
		std::ofstream dst(module_name_temp, std::ofstream::binary | std::ofstream::trunc);

		if(!dst)
		{
			std::cout<< "Error: file" <<module_name << "cannot be open..." << std::endl;
			return false;
		}

		//Copy (rewrite) file
		dst << src.rdbuf();

		src.close();
		dst.close();

		//Open updated module
		module.open(module_name_temp);
		//Remove
		if(std::remove(module_name.c_str()) != 0)
		{
			std::cout << "Error: file" << module_name << "could not be removed..." << std::endl;
			return false;
		}
		//Rename 
		if (std::rename(module_name_temp.c_str(), module_name.c_str() ) != 0)
		{
				std::cout << "Error: file '" << module_name_temp << "' could not be renamed;" << std::endl;
				return false;
		}
		//Open Module
		if (module.is_open() == false)
		{
				std::cout << "Error: the application module '" << module_name << "' can not be opened;" << std::endl;
				return false;
		}

		void *(*addr)(void *) = nullptr;

		//Application Call
		if(module.find("application_call", &addr) == false)
		{
			std::cout << "Error: function 'application_call' was not found in the module"<< module_name <<"..."<<std::endl;
			return false;
		}
		
		std::function <int (
										Transfer::app_request *,Transfer::app_response *
										)> app_call = reinterpret_cast <int(*)(
										Transfer::app_request *,Transfer::app_response *				
										)> (addr);

		if(!app_call)
		{
			std::cout << "Error: invalid function 'application_call' is in the module"<<module_name<<std::endl;
			return false;
		}
		//Application_clear
		if(module.find("application_clear", &addr) == false)
		{
			std::cout<< "Error: function 'application_clear' was not found int the module" << module_name<<std::endl;
			return false;
		}

		std::function<void (void*, size_t)> app_clear = reinterpret_cast<void(*)(void *, size_t)>(addr);

		//Application Init
		std::function<bool(const char*)> app_init = std::function<bool(const char*)>();
		if(module.find("application_init", &addr))
		{
			app_init = reinterpret_cast <bool (*)(const char *)>(addr);
		}
		
		std::function<void (const char*)> app_final= std::function <void (const char*)>();
		//Application Final
		if(module.find("application_final", &addr))
		{
			app_final = reinterpret_cast <void (*)(const char *)>(addr);
		}

		for(auto &app : same)
		{
			app->application_call = app_call;
			app->application_clear = app_clear;
			app->application_init = app_init;
			app->application_final = app_final;

			try
			{
				if(app->application_init)
				{
					const std::string root = app->root_dir;
					app->application_init(root.data() );
				}
			}
			catch(std::exception &exc)
			{
				std::cout<< "Warning : an exception was thrown when the application"<<module_name<<std::endl;
			}
		}
		return true;
	}

	void Server:: threadRequestCycle(
										SocketsQueue &sockets,
										Utils::Event &eventThreadCycle
										) const
		{
			while(true)
			{
				Socket::Socket sock;
				Http2::IncStream * stream = nullptr;

				eventThreadCycle.wait();

				if(this->controls.process_flag == false)
				{
					break;
				}

				sockets.lock();

				if(sockets.size())
				{
					std::tie(sock, stream) = sockets.front();

					sockets.pop();
				}

				sockets.unlock();

				if(sock.is_open())
				{
					++this->threads_working_count;

					::sockaddr_in sock_addr{};
					::socklen_t sock_addr_len = sizeof(sock_addr);

					::getsockname(
						sock.get_handle(),
						reinterpret_cast <sockaddr *> (&sock_addr),
						&sock_addr_len
					);

					const int port = ntohs(sock_addr.sin_port);

					auto const it = this->tls_data.find(port);

					if(this->tls_data.cend() != it)
					{
						if(stream)
						{
							Socket::AdapterTls socket_adapter(
								reinterpret_cast <gnutls_session_t> (stream -> reserved)
							);

							this->threadRequestProc(
								socket_adapter,
								sockets,
								stream
							);
						}
						else 
						{
							const std::tuple <gnutls_certificate_credentials_t, gnutls_priority_t> &data = it->second;
							Socket::AdapterTls socket_adapter(
								sock,
								std::get<gnutls_priority_t> (data),
								std::get<gnutls_certificate_credentials_t>(data)
							);

							if(socket_adapter.handshake())
							{
								this->threadRequestProc(
									socket_adapter,
									sockets,
									nullptr
								);
							}
						}
					}
					else 
					{
						Socket::AdapterDefault socket_adapter(sock);

						this->threadRequestProc(
							socket_adapter,
							sockets,
							stream
						);

					}
					--this->threads_working_count;
				}
			}
		}

	void Server:: threadRequestProc(
				Socket::Adapter &sock,
				SocketsQueue &sockets,
				Http2::IncStream *stream
				)const
	{
		std::unique_ptr<ServerProtocol> prot = getProtocolVariant(
			sock,
			this->settings,
			this->controls,
			sockets,
			stream
		);

		if(prot)
		{
			//Check if switching protocol
			for(ServerProtocol *ret = nullptr; ;)
			{
				ret = prot -> process();

				if(prot.get() == ret)
				{
					break;
				}
				prot.reset(ret);
			}
			prot->close();
		}
	}

	static void close_listeners(std::vector<Socket::Socket> &listeners )
	{
		for(auto & sock: listeners)
		{
			sock.close();
		}
	}

	void Server::stop()
	{
		this->controls.stopProcess();
		close_listeners(this->listeners);
	}	

	void Server:: restart()
	{
		this->controls.setRestart();
		this->controls.stopProcess();

		close_listeners(this->listeners);
		
	}

	void Server::update()
	{
		this->controls.setUpdateModule();
		this->controls.setProcess(false);
		this->controls.setProcessQueue();
	}

	void Server:: clear()
	{
		this->controls.clear();

		if (this->tls_data.empty() == false)
		{
			for (auto &pair : this->tls_data)
			{
				std::tuple<gnutls_certificate_credentials_t, gnutls_priority_t> &data = pair.second;
				
				::gnutls_certificate_free_credentials(std::get<gnutls_certificate_credentials_t>(data) );
				::gnutls_priority_deinit(std::get<gnutls_priority_t>(data) );
			}
		}
		
		this->settings.clear();
		
		if (this->modules.empty() == false)
		{
			for (auto &module : this->modules)
			{
				module.close();
			}
			
		this->modules.empty();
		}
		
		::gnutls_global_deinit();

		Socket::Socket::Cleanup();
	}

	bool Server::init()
	{
		ConfigParser conf_parser;
		std::cout << "Initializing ..." << std::endl;
		if (Socket::Socket::Startup() && conf_parser.loadConfig("main.conf", this->settings, this->modules) )
		{	
			this->settings.addDataVariant(new DataVariant::FormUrlencoded() );
			this->settings.addDataVariant(new DataVariant::MultipartFormData() );
			this->settings.addDataVariant(new DataVariant::TextPlain() );

			::gnutls_global_init();
			std::cout << "Initialization Succeeded..." << std::endl;
			return true;
		}
		std::cout << "Initialization Failed..." << std::endl;
		return false;
	}

	//------------------------Other-----------------------//
	System::native_processid_type Server::getServerProcessId(const std::string &serverName)
	{
		System::native_processid_type pid =0;
	
		System::GlobalMutex glob_mtx;

		if(glob_mtx.open(serverName))
		{
			System::SharedMemory glob_mem;

			glob_mtx.lock();

			if(glob_mem.open(serverName))	//Set shm_desc with serverName
			{
				glob_mem.read(&pid, sizeof(pid)); //PID is written at front of Shared Memory 
			}
			glob_mtx.unlock();
		}

		return pid;
	}

	static std::string get_server_name(const int argc, const char* argv[])
	{
		std::string server_name;

		for(int i=0; i< argc; ++i)
		{
			if(argv[i] == ::strstr(argv[i], "--server-name=") )
			{
				//Server name is typed in as parameter
				server_name = std::string(argv[i] + sizeof("--server-name=")-1);
				break;
			}
		}
			if(server_name.empty())
			{
				//Take first program name as Server_name
				server_name = argv[0];
			}

		System::filterSharedMemoryName(server_name);
		return server_name;
	
	}

	bool Server::get_start_args(
				const int argc,
				const char*argv[],
				struct server_start_args * st) 
	{
		for(int i =1; i< argc; ++i)
		{
			if( 0 == ::strcmp(argv[i], "--start"))
			{
			}
			else if(0 == ::strcmp(argv[i], "--force"))
			{
					st->force = true;	
			}
			else if(argv[i] == ::strstr(argv[i], "--config-path=")) //Config path,following '--config-path='
			{	
				st->config_path = std::string(argv[i] + sizeof("--config-path=") -1);
			}
			else if(argv[i] == ::strstr(argv[i], "--server-name"))	//Server name
			{
				st->server_name = std::string(argv[i] + sizeof("--server-name=")-1);
			}
			else 
			{
				std::cout<< "Argument '" << argv[i] << "' can`t be applied with --start;" << std::endl;
				return false;
			}

			if(st -> server_name.empty())
			{
				st -> server_name = argv[0];
			}
			System::filterSharedMemoryName(st->server_name);	
		}
			return true;
	}
	//----------------------Command_line----------------------------//

	int Server:: command_help(const int argc, const char*argv[]) const
	{
 		 std::cout<<"fetching help info..."<<std::endl;

		std::cout << std::left << "Available arguments:" << std::endl
				<< std::setw(4) << ' ' << std::setw(26) << "--start" << "Start http server" << std::endl
				<< std::setw(8) << ' ' << std::setw(22) << "[options]" << std::endl
				<< std::setw(8) << ' ' << std::setw(22) << "--force" << "Forcibly start http server (ignore existing instance)" << std::endl
				<< std::setw(8) << ' ' << std::setw(22) << "--config-path=<path>" << "Path to directory with configuration files" << std::endl
				<< std::endl
				<< std::setw(4) << ' ' << std::setw(26) << "--restart" << "Restart http server" << std::endl
				<< std::setw(4) << ' ' << std::setw(26) << "--update-module" << "Update applications modules" << std::endl
				<< std::setw(4) << ' ' << std::setw(26) << "--kill" << "Shutdown http server" << std::endl
				<< std::setw(4) << ' ' << std::setw(26) << "--help" << "This help" << std::endl
				<< std::endl<< "Optional arguments:" << std::endl
				<< std::setw(4) << ' ' << std::setw(26) << "--server-name=<name>" << "Name of server instance" << std::endl;

			return EXIT_SUCCESS;
	}

	int Server::command_start(const int argc, const char*argv[])
	{
		std::cout<<"starting..." << std::endl;

		struct server_start_args st = {};
		//Check Start Command 
		if(Server::get_start_args(argc,argv,&st) == false)
		{
			return 0x1;
		}
		//Change Current Directory
		if(st.config_path.empty() == false )		
		{
			if(System::changeCurrentDirectory(st.config_path) == false)
			{
				std::cout << "Configuration path "<< st.config_path<<" not found ..." << std::endl;
				return 0x2;
			}
		}	
		
		System::GlobalMutex glob_mtx;
		System::SharedMemory glob_mem;

		//If force, destroy anything exists and start
		if(st.force)
		{
			System::SharedMemory :: destroy (st.server_name);
			System::GlobalMutex :: destroy (st.server_name); 
		}
		//Check is already on running
		bool is_exists = false;

		if(glob_mtx.open(st.server_name))
		{
			glob_mtx.lock();

			if(glob_mem.open(st.server_name))
			{
				System::native_processid_type pid = 0;
				if(glob_mem.read( &pid, sizeof(pid)))
				{
					is_exists = System::isProcessExists(pid);
				}
			}
			glob_mtx.unlock();
		}

		if(is_exists) 
		{
			std::cout<< "Server Instance with name " << st.server_name << " Already running"<<std::endl;
			return 0x3;
		}
		//Creating GlobalMutex
		if(glob_mtx.open(st.server_name) == false)
		{
			if(glob_mtx.create(st.server_name) == false)
			{
				std::cout<<"Global mutex could not be created..."<<std::endl;
				return 0x4;
			}
		}
		//Creating SharedMemory
		glob_mtx.lock();

		if(glob_mem.open(st.server_name) == false)
		{
			if(glob_mem.create(st.server_name,sizeof(System::native_processid_type)) == false)
			{
				glob_mtx.unlock();
				std::cout<<"Shared memory could not be allocated..."<<std::endl;
				return 0x5;
			}
		}
		//Store PID at front of sharedMemory
		System::native_processid_type pid = System::getProcessId();

		if(glob_mem.write(&pid, sizeof(pid)) == false)
		{
			glob_mem.destroy();
			glob_mtx.unlock();
			std::cout<<"Writing data to shared memory failed..."<<std::endl;
			return 0x6;
		}
		glob_mtx.unlock();
		//Start running
		int exitcode = EXIT_FAILURE;

		do{
			this->controls.setProcess(false);
			this->controls.setRestart(false);
			
			exitcode = this->run();
		}
		while(this->controls.process_flag || this->controls.restart_flag);
		//Clear
		glob_mem.destroy();
		glob_mtx.destroy();

		return exitcode;
	}

	int Server::command_restart(const int argc, const char*argv[]) const
	{
		std::cout << "restart..."<<std::endl;

		const System::native_processid_type pid = Server::getServerProcessId(get_server_name(argc,argv));
		if( 1< pid && System::sendSignal(pid,SIGUSR1))
		{
			return EXIT_SUCCESS;
		}
		return EXIT_FAILURE;
	}	

	int Server::command_terminate(const int argc, const char*argv[]) const
	{
		std::cout<<"Terminate..."<< std::endl;

		const System::native_processid_type pid = Server::getServerProcessId(get_server_name(argc,argv));
		if( 1<pid && System::sendSignal(pid, SIGTERM))   //Pid=1: Init process
		{
			return EXIT_SUCCESS;
		}
		return EXIT_FAILURE;
	}

	int Server::command_update_module(const int argc, const char*argv[]) const
	{	
		std::cout<< "update module..."<< std::endl;
		const System::native_processid_type pid = Server::getServerProcessId(get_server_name(argc,argv));
		if( 1< pid && System::sendSignal(pid,SIGUSR2))
		{
			return EXIT_SUCCESS;
		}
		return EXIT_FAILURE;
	}


}
