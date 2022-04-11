#include "NetSocketUV.hpp"

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

bool NetSocketUV::Accept()
{
	NetSocketUV* accept_sock = NewSocket(net);
	accept_sock->Create(0, true, false);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);
	uv_tcp_t* host = GetPtrTCP(sock);
	if (uv_accept((uv_stream_t*)host, (uv_stream_t*)client) == 0)
	{	
		//std::thread receivThread(StartReadingThread, client);
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP) == 0)
		{
			ServerUV* server = ((ServerUV*)net);
			server->count_accept++;
			server->ConnectSocket(accept_sock, server->count_accept);
			printf("Accepted client with ID:%d\n", accept_sock->GetClientID());
			server->sockets_nohello.Add(accept_sock);
			std::thread translateThread(SetupRetranslation, client);
			translateThread.detach();
			return true;
		}
		//receivThread.join();
		else
			return false;
	}
	else
	{
		std::cout << "Cannot accepting client!" << std::endl;
		return false;
	}
	return false;
}

void NetSocketUV::ReceiveTCP()
{
	int filePrefix = GetClientID();
	printf("ID: %d\n", filePrefix);
	/*std::array<char, 10> strID;
	std::to_chars(strID.data(), strID.data() + strID.size(), filePrefix);*/
	CString fileName(filePrefix);
	fileName += "in_binary_h.264";
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
	if (max_length < 800000)
		recv_buffer->SetMaxSize(800000);
	buf->len = 800000;
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
	net_sock->Accept();
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

void* NetSocketUV::SetupRetranslation(void* argv)
{
	NetSocketUV* socket = (NetSocketUV*)GetNetSocketPtr(argv);
	std::this_thread::sleep_for(std::chrono::microseconds(1000));
	assert(socket);
	if (socket->GetClientID())
	{
		/*unsigned int ID = socket->GetClientID();
		std::array<char, 10> strID;
		std::to_chars(strID.data(), strID.data() + strID.size(), ID);
		std::string fileName(strID.data());
		fileName += "in_binary_h.264";*/
		int filePrefix = socket->GetClientID();
		CString fileName(filePrefix);
		fileName += "in_binary_h.264";
		if (std::filesystem::exists((std::string)fileName) == true) {
			
			FILE* proxy = nullptr;
#ifdef WIN32
			//system("RTSPProxyServerForClient.exe -d -c -%s");
			proxy = _popen("RTSP.exe -d -c -%s", "r");
			_pclose(proxy);		
			
#else
			int status;
			pid_t pid;

			pid = fork();
			std::string outRTSP;
			/* Handeling Chile Process */
			
			if (pid == 0) {
				char* execv_str[] = { "./RTSP", (char*)fileName.c_str(), NULL};
				if (execv("./RTSP", execv_str) < 0) {
					status = -1;
					perror("ERROR\n");
				}
				else
				{
					printf("Start translation success!\n");
					std::getline(std::cin, outRTSP);
					if (outRTSP.find("rtsp://"))
					{
						std::thread delay(WaitingDelay, &socket);
						//delay.join();
						delay.detach();
					}
				}
			}
			/* Handeling Chile Process Failure */
			else if (pid < 0) {
				status = -1;
				perror("ERROR\n");
			}
#endif
		}
	}
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
	return (serv.getSockLoop());
}

unsigned int timer_count = 0;
ServerUV* uv_server = NULL;
ServerUV::ServerUV()
{
	timer_count = 0;
	memset(&loop, 0, sizeof(loop));

	int r = uv_loop_init(&loop);
	assert(r == 0);
	uv_server = this;
}

ServerUV::~ServerUV()
{
	Destroy();
}

void ServerUV::Destroy()
{
	Server::Destroy();
	uv_server = NULL;

	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
}

void ServerUV::StopServer()
{
	uv_stop(&loop);
}

NetSocket* ServerUV::NewSocket(Net* net)
{
	return new NetSocketUV(net);
}

NET_BUFFER_INDEX* ServerUV::NewBuffer(int index)
{
	return new NetBufferUV(index);
}

void ServerUV::StartUVServer(bool internet)
{
	if (internet)
	{
		bool res = Create(true);
		if (res)
		{
			printf("Success create server!\n");
			UpdateNet();
		}
	}
}

bool ServerUV::UpdateNet()
{
	uv_loop_t* loop = GetLoop(this);

	uv_run(loop, UV_RUN_DEFAULT);
	return false;
}
