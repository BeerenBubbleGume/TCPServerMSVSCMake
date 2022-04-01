#include "NetSocketUV.hpp"

#define FRAME_SIZE 150000

using std::ofstream;
ofstream fout;

NetSocketUV::NetSocketUV(Net* net) : NetSocket(net)
{
	sock = NULL;
	status = errno;	
	loop = uv_default_loop();
}


NetSocketUV::~NetSocketUV()
{
	status = 0;
}

bool NetSocketUV::Create(int port, bool udp_tcp, bool listen)
{	
	NetSocket::Create(port, udp_tcp, listen);
	uv_loop_t* sloop = GetLoop(net);
	if (sloop)
	{
		
		if (udp_tcp)
		{
			sock = new TCP_SOCKET;
			memset(sock, 0, sizeof(TCP_SOCKET));
			((TCP_SOCKET*)sock)->net_socket = this;
			
			uv_tcp_t* tcp = GetPtrTCP(sock);

			int r = uv_tcp_init(loop, tcp);
			assert(r == 0);

			r = uv_tcp_nodelay(tcp, true);
			assert(r == 0);

			if (listen)
			{
				sockaddr_in* sock_addres = new sockaddr_in;
				
				int i = uv_ip4_addr("0.0.0.0", port, sock_addres);
				assert(i == 0);
				int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
				assert(b == 0);

				int l = uv_listen((uv_stream_t*)tcp, 10240, OnAccept);
				if (l != 0)
					return false;
				return uv_run(sloop, UV_RUN_DEFAULT);
			}																   
				
		}
		else
		{
			sock = malloc(sizeof(UDP_SOCKET));
			memset(sock, 0, sizeof(UDP_SOCKET));
			int r = uv_udp_init(loop, GetPtrUDP(sock));
			assert(r == 0);

			struct sockaddr_in broadcast_addr;
			uv_ip4_addr(addr->address, addr->port, &broadcast_addr);
			r = uv_udp_bind(GetPtrUDP(sock), (const struct sockaddr*)&broadcast_addr, 0);
			assert(r == 0);

			r = uv_udp_set_broadcast(GetPtrUDP(sock), 1);
			assert(r == 0);

			if (listen)
			{
				r = uv_udp_recv_start(GetPtrUDP(sock), OnAllocBuffer, OnReadUDP);
				assert(r == 0);
			}

			((UDP_SOCKET*)sock)->net_socket = this;
		}
		return true;
	}
	else
	{
		std::cout << "Loop cannot been initialized!" << std::endl;
		return false;
	}
}

const char* NetSocketUV::GetIP(Net_Address* addr, bool own_or_peer)
{
	if (own_or_peer)
	{
		std::cout << "Set IP to socket!" << std::endl;
		udp_tcp = true;
		struct addrinfo hints, *info, *p;
		int gai_result;

		char hostname[1024];
		hostname[1023] = '\0';
		gethostname(hostname, 1023);

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_CANONNAME;
		if ((gai_result = getaddrinfo(hostname, "rtsp", &hints, &info)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
			exit(1);
		}

		for (p = info; p != NULL; p = p->ai_next) {
			printf("hostname: %s\n", p->ai_canonname);
		}
		

		freeaddrinfo(info);
	}
	else
		return nullptr;
}

bool NetSocketUV::Accept(uv_handle_t* handle)
{
	NetSocketUV* accept_sock = NewSocket(net);
	accept_sock->Create(0, true, false);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);

	if (uv_accept((uv_stream_t*)handle, (uv_stream_t*)client) == 0)
	{
		sockaddr sockname;
		int socklen = sizeof accept_sock->net->GetConnectSockaddr();
		std::thread translateThread(SetupRetranslation, client);
		//std::thread receivThread(StartReadingThread, client);
		translateThread.detach();
		uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP);

		//receivThread.join();
		/*uv_thread_t receivThread;
		uv_thread_t translateThread;

		uv_thread_create(&receivThread, StartReadingThread, client);
		uv_thread_create(&translateThread, SetupRetranslation, this);

		uv_thread_join(&receivThread);
		uv_thread_join(&translateThread);*/
		
		
		//uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP);
	}
	else
	{
		std::cout << "Cannot accepting client!" << std::endl;
		return false;
	}
	return false;
}

void NetSocketUV::SendTCP(NET_BUFFER_INDEX *buf)
{
	
}

void NetSocketUV::SendUDP(NET_BUFFER_INDEX *buf)
{
}

void NetSocketUV::ReceiveTCP()
{
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	rbuff.Add(received_bytes, recv_buffer->GetData());
	
	/*CString fileName;
	unsigned int filePrefix = GetClientID();
	if ((fileName + filePrefix).Find('0'))
	{
		fileName = ("0in_binary_h.264");
		fout.open(fileName.c_str(), std::ios::binary | std::ios::app);
		if (fout.is_open())
		{
			fout.write((char*)net->GetRecvBuffer()->GetData(), net->GetRecvBuffer()->GetLength());
			printf("writed %d bytes in file %s\n", (int)net->GetRecvBuffer()->GetLength(), fileName.c_str());
			fout.close();
		}
		else
		{
			printf("cannot open file\n");
		}
	}
	else
	{
		char* name = "in_binary_h.264";
		fileName.IntToString(filePrefix);
		
		fileName += name;
		fout.open(fileName.c_str(), std::ios::binary | std::ios::app);
		if (fout.is_open())
		{
			fout.write((char*)net->GetRecvBuffer()->GetData(), net->GetRecvBuffer()->GetLength());
			printf("writed %d bytes in file %s\n", (int)net->GetRecvBuffer()->GetLength(), fileName.c_str());
			fout.close();
		}
		else
		{
			printf("cannot open file\n");
		}
	}*/
}

void NetSocketUV::ReceiveUPD()
{
}

void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(stream);
	uvsocket->getNet()->setupReceivingSocket(*uvsocket);
	printf("Reading data from client with ID: %d\n", uvsocket->GetClientID());
	if (nread < 0)
	{
		printf("read buffer < 0!\n");
		uvsocket->getNet()->OnLostConnection(uvsocket);
		OnCloseSocket((uv_handle_t*)stream);
	}
	else
	{
		NetBuffer* recv_buff = uvsocket->getNet()->GetRecvBuffer();
		assert(buf->base == (char*)recv_buff->GetData());
		recv_buff->SetMaxSize(nread);
		uvsocket->ReceiveTCP();
	}
}

void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = net_sock->getNet()->GetRecvBuffer();

	unsigned int max_length = recv_buffer->GetMaxSize();
	if (max_length < suggested_size)
		recv_buffer->SetMaxSize(suggested_size);
	buf->len = suggested_size;
	buf->base = (char*)recv_buffer->GetData();
}

void OnCloseSocket(uv_handle_t *handle)
{
	char* ptr = (char*)handle;
	ptr -= sizeof(void*);
	free(ptr);
}

void OnWrite(uv_write_t *req, int status)
{
	int offset = offsetof(NetBufferUV, sender_object);
	NetBufferUV* buf = (NetBufferUV*)(((char*)req) - offset);
	NET_BUFFER_LIST* list = (NET_BUFFER_LIST*)buf->owner;
	int index = buf->GetIndex(); 
	NetSocketUV* uvsocket = (NetSocketUV*)list->net->getReceivingSocket();		
	
	list->DeleteBuffer(index);
}

void StartReadingThread(void* handle)
{
	uv_read_start((uv_stream_t*)handle, OnAllocBuffer, OnReadTCP);
}

char address_converter[30];
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
	NetSocket* socket = GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = socket->getNet()->GetRecvBuffer();
	assert(buf->base == (char*)recv_buffer->GetData());
	recv_buffer->SetLength(nread);

	int r = uv_ip4_name((sockaddr_in*)addr, address_converter, sizeof(address_converter));
	socket->getNet()->addr->address = address_converter;
	unsigned char* port_ptr = (unsigned char*)&(((sockaddr_in*)addr)->sin_port);
	socket->getNet()->addr->port = port_ptr[1];
	socket->getNet()->addr->port += port_ptr[0] << 8;

	socket->ReceiveUPD();
}

void OnAccept(uv_stream_t* stream, int status)
{
	std::cout << "___ On Connect ___" << std::endl;
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(stream);
	net_sock->Accept((uv_handle_t*)stream);

}

void NetSocketUV::Destroy()
{
	if (sock)
	{
		if (udp_tcp)
		{
			uv_close((uv_handle_t *)GetPtrTCP(sock), OnCloseSocket);
			((TCP_SOCKET *)sock)->net_socket = nullptr;
		}
		else
		{
			int r = uv_read_stop((uv_stream_t *)GetPtrUDP(sock));
			assert(r == 0);
			uv_close((uv_handle_t *)sock, OnCloseSocket);
			((UDP_SOCKET *)sock)->net_socket = NULL;
		}
		sock = NULL;
	}
	NetSocket::Destroy();
}

void RTSPProxyServer::play(void* sock_ptr)
{
	NetSocketUV* socket = (NetSocketUV*)&sock_ptr;
	u_int8_t* buffer = socket->GetReceivingBuffer()->GetData();
	u_int64_t len = socket->GetReceivingBuffer()->GetLength();

	in_addr groupAddr;
	groupAddr.s_addr = AF_INET;
	portNumBits rtpPortNum(1888);
	Port rtpPort(rtpPortNum);
	Groupsock* rtpGS = new Groupsock(envir(), *(sockaddr_storage*)&groupAddr, rtpPort, 225);

	ByteStreamMemoryBufferSource* source = ByteStreamMemoryBufferSource::createNew(envir(), buffer, FRAME_SIZE);
	H264VideoRTPSink* sink = H264VideoRTPSink::createNew(envir(), rtpGS, 96);

	sink->startPlaying(*source, proxyServerMediaSubsessionAfterPlaying, this);
}

void* NetSocketUV::SetupRetranslation(void* argv)
{
	NetSocketUV* socket = (NetSocketUV*)GetNetSocketPtr(argv);
	std::this_thread::sleep_for(std::chrono::microseconds(500));
	assert(socket);

	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	RTSPProxyServer* proxyserv = RTSPProxyServer::createNew(*env, 8554);
	if (proxyserv == nullptr)
	{
		printf("cannot create RTSP server!");
		exit(1);
	}

	unsigned int ID = socket->GetClientID();
	std::array<char, 10> strID;
	std::to_chars(strID.data(), strID.data() + strID.size(), ID);
	std::string streamName("ServerRTSP/Channals/");
	streamName += strID.data();

	ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName.c_str());
	sms->addSubsession(DemandServerMediaSubsession::createNew(*env, false));
	proxyserv->addServerMediaSession(sms);
	proxyserv->play(&socket);
	proxyserv->anonceStream(proxyserv, sms, streamName.c_str());
//	if (socket->GetClientID())
//	{
//		unsigned int ID = socket->GetClientID();
//		std::array<char, 10> strID;
//		std::to_chars(strID.data(), strID.data() + strID.size(), ID);
//		std::string fileName(strID.data());
//		fileName += "in_binary_h.264";
//		if (std::filesystem::exists(fileName) == true) {
//			
//			FILE* proxy = nullptr;
//#ifdef WIN32
//			//system("RTSPProxyServerForClient.exe -d -c -%s");
//			proxy = _popen("RTSP.exe -d -c -%s", "r");
//			_pclose(proxy);		
//
//#else
//			int status;
//			pid_t pid;
//
//			pid = fork();
//			std::string outRTSP;
//			/* Handeling Chile Process */
//			//if (ID == "0")
//			//{
//			//	if (pid == 0) {
//			//		char* execv_str[] = { "./RTSP", strID.data()};
//			//		if (execv("./RTSP", execv_str) < 0) {
//			//			status = -1;
//			//			perror("ERROR\n");
//			//		}
//			//		else
//			//		{
//			//			std::getline(std::cin, outRTSP);
//			//			if (outRTSP.find("rtsp://"))
//			//			{
//			//				std::thread delay(WaitingDelay, socket);
//			//				delay.join();
//			//				//delay.detach();
//			//			}
//			//		}
//			//	}
//			//	/* Handeling Chile Process Failure */
//			//	else if (pid < 0) {
//			//		status = -1;
//			//		perror("ERROR\n");
//			//	}
//			//}
//			//else
//			//{
//				if (pid == 0) {
//					char* execv_str[] = { "./RTSP", strID.data()};
//					if (execv("./RTSP", execv_str) < 0) {
//						status = -1;
//						perror("ERROR\n");
//					}
//					else
//					{
//						std::getline(std::cin, outRTSP);
//						if (outRTSP.find("rtsp://"))
//						{
//							std::thread delay(WaitingDelay, socket);
//							delay.join();
//							//delay.detach();
//						}
//					}
//				}
//				/* Handeling Chile Process Failure */
//				else if (pid < 0) {
//					status = -1;
//					perror("ERROR\n");
//				}
//			//}
//#endif
//		}
//	}

	env->taskScheduler().doEventLoop();

	return 0;
}

void* NetSocketUV::WaitingDelay(void* delay)
{
	
	if (delay != nullptr)
	{
		std::this_thread::sleep_for(std::chrono::minutes(10));
		NetSocketUV* socket = (NetSocketUV*)delay;
		socket->Destroy();
		return nullptr;
	}
	else
	{
		printf("delay <= 0!");
		return 0;
	}
}

Server::Server()
{
	net = new Net;
	net_sockuv = new NetSocketUV(net);
}

Server::~Server()
{
	if (net)
		delete net;
	if (net_sockuv)
		delete net_sockuv;
}

int Server::connect(bool connection)
{
	if (connection)
	{
		//udp_tcp = true;
		std::cout << "==========Start server!==========" << std::endl;
		//net_sockuv->GetIP(NULL, true);
		if (net_sockuv->Create(1885, true, true) == true)
			return 0;
	}
	else
	{
		fprintf(stderr, "Server is not connetcted!\n");
		return 1;
	}
}

uv_tcp_t *GetPtrTCP(void *ptr)
{
	return (uv_tcp_t*)(((char*)ptr) + sizeof(void*));
}

uv_udp_t *GetPtrUDP(void *ptr)
{
	return (uv_udp_t *)(((char *)ptr) + sizeof(void *));
}

uv_loop_t *GetLoop(Net* net)
{
	NetSocketUV serv = (NetSocketUV)net;
	return (serv.loop);
}

