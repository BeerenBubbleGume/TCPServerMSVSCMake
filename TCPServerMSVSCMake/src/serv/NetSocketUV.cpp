#include "NetSocketUV.hpp"
//#pragma comment(lib, "../../libs/lib/uv.lib")


using std::ofstream;


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
				
				int i = uv_ip4_addr("127.0.0.1", port, sock_addres);
				assert(i == 0);
				int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
				assert(b == 0);
				/*uv_thread_t acceptingThread;
				int l = uv_thread_create(&acceptingThread, OnListining, tcp);
				assert(l == 0);
				l = uv_thread_join(&acceptingThread);*/
				int l = uv_listen((uv_stream_t*)tcp, 1024, OnAccept);
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
		//int curID = uv_tcp_getsockname(client, &accept_sock->net->GetConnectSockaddr(), &socklen);
		accept_sock->SetID(client);
		//std::cout << "Start accepting RTSP from: " << curID << std::endl;
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP) == 0)
		{	
			return true;
		}
		
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
	if (buf->GetLength() > 0)
	{
		uv_buf_t buffer;
		buffer.len = buf->GetLength();
		buffer.base = (char*)buf->GetData();
		uv_buf_init(buffer.base, buffer.len);
		
	}
	//OnCloseSocket((uv_handle_t*)GetPtrTCP(sock));
}

void NetSocketUV::SendUDP(NET_BUFFER_INDEX *buf)
{
}

void NetSocketUV::ReceiveTCP()
{
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	recv_buffer->Add(received_bytes, (void*)recv_buffer->GetData());
	
	std::ofstream fout;
	fout.open("out_h.264", std::ios::binary);
	if (fout.is_open())
	{
		std::cout << "start recording stream in file" << std::endl;
		fout.write((const char*)recv_buffer->GetData(), received_bytes);
		fout.close(); 
		recv_buffer->Delete(received_bytes);
	}
	else
	{
		std::cout << "cannot open file to record stream!" << std::endl;
	}
	FILE* proxy = nullptr;
#ifdef WIN32
	//system("RTSPProxyServerForClient.exe -d -c -%s");
	proxy = _popen("RTSP.exe -d -c -%s", "r");
	_pclose(proxy);
#else
	//system("./RTSPProxyServerForClient -d -c -%s");
	//proxy = popen("./RTSP -c -%s", "r");
	//pclose(proxy);

	pid_t pid, cmd;
	int pin[2], pout[2]; // пара пайпов для обмена со скриптом
	if (pipe(pin) || pipe(pout))
		err(EX_OSERR, "pipes");

	if (!(cmd = fork())) { // запуск скрипта с аргументами
		close(pin[1]);
		close(pout[0]);
		dup2(pin[0], 0);     // заменим stdin скрипта на наш ввод
		dup2(pout[1], 1);    // перекинем stdout скрипта на наc
							 // stderr скрипта остался тем же, что и у нас
		execvp("./RTSP", (char* const*)GetClientID());
		err(EX_UNAVAILABLE, "exec %s", "RTSP");
	}
	if (cmd == -1)
		err(EX_OSERR, "fork");

	// эти дескрипторы более не нужны
	close(pin[0]);
	close(pout[1]);

	char* str = 0;
	size_t ssize = 0;
	ssize_t l;

	// для примера читаем stdin и передаем в скрипт
	while ((l = getline(&str, &ssize, stdin)) > 0)
		if (write(pin[1], str, l) != l)
			err(EX_IOERR, "write pipe");

	close(pin[1]); // пошлем EOF на stdin скрипта

	// читаем вывод скрипта и для примера печатаем его
	FILE* progout = fdopen(pout[0], "r");
	while (getline(&str, &ssize, progout) > 0)
		puts(str);

	free(str);
	int status;
	errno = 0;
	if ((pid = wait(&status)) != cmd) {
		if (errno)
			err(EX_SOFTWARE, "wait");
		else
			errx(EX_SOFTWARE, "wait unexpected PID %ld (waited %ld)",
				(long)pid, (long)cmd);
	}
	if (WIFEXITED(status))
		printf("%s exit code %d\n", "RTSP", WEXITSTATUS(status));
	else
		printf("%s terminated by %d\n", "RTSP", WTERMSIG(status));

	//return puts("End") == EOF;
#endif // WIN32
	

	/*NET_BUFFER_INDEX* index = net->PrepareMessage(10, received_bytes, recv_buffer->GetData());
	assert(index);
	SendTCP(index);*/
}

void NetSocketUV::ReceiveUPD()
{
}

void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(stream);
	uvsocket->net->setupReceivingSocket(*uvsocket);
	std::cout << "Receiving..." << std::endl;
	if (nread < 0)
	{
		std::cout << "read buff < 0" << std::endl;
		uvsocket->net->OnLostConnection(uvsocket);
		OnCloseSocket((uv_handle_t*)stream);
	}
	else
	{
		std::cout << "Reading UV socket from client with ID:" << uvsocket->GetClientID() << std::endl;
		NetBuffer* recv_buff = uvsocket->net->GetRecvBuffer();
		assert(buf->base == (char*)recv_buff->GetData());
		recv_buff->SetMaxSize(nread);
		uvsocket->ReceiveTCP();
	}
}

void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = net_sock->net->GetRecvBuffer();

	unsigned int max_length = recv_buffer->GetMaxSize();
	if (max_length < suggested_size)
		recv_buffer->SetMaxSize(suggested_size);
	buf->len = suggested_size;
	buf->base = (char*)recv_buffer->GetData();

	//recv_buffer->Add(buf->len, buf->base);
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

char address_converter[30];
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
	NetSocket* socket = GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = socket->net->GetRecvBuffer();
	assert(buf->base == (char*)recv_buffer->GetData());
	recv_buffer->SetLength(nread);

	int r = uv_ip4_name((sockaddr_in*)addr, address_converter, sizeof(address_converter));
	socket->net->addr->address = address_converter;
	unsigned char* port_ptr = (unsigned char*)&(((sockaddr_in*)addr)->sin_port);
	socket->net->addr->port = port_ptr[1];
	socket->net->addr->port += port_ptr[0] << 8;

	socket->ReceiveUPD();
}

void OnListining(void* tcp)
{
	int l = uv_listen((uv_stream_t*)tcp, 1024, OnAccept);
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
	//outFile.close();
	NetSocket::Destroy();
}


void HandlerForRegister(void* clientData)
{
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
		if (net_sockuv->Create(8554, true, true) == true)
			return 0;
	}
	else
	{
		fprintf(stderr, "Server is not connetcted!\n");
		return 1;
	}
}

uv_poll_t* GetPtrPoll(void* ptr)
{
	return (uv_poll_t*)(((char*)ptr) + sizeof(void*));
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

