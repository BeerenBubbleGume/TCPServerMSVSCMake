#include "NetSocketUV.hpp"
//#pragma comment(lib, "../../libs/lib/uv.lib")

using std::ofstream;
ofstream fout;

NetSocketUV::NetSocketUV(Net* net) : NetSocket(net)
{
	sock = NULL;
	status = errno;	
	loop = uv_default_loop();
	memset(&fs_data, 0, sizeof fs_data);
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
		accept_sock->SetID(client);
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
	uv_buf_t buffer;
	buffer.base = "REGISTER";
	buffer.len = sizeof buffer.base;
	uv_buf_init(buffer.base, buffer.len);
	
	uv_write(((NetBufferUV*)buf)->GetPtrWrite(), (uv_stream_t*)GetPtrTCP(sock), &buffer, 1, OnWrite);
}

void NetSocketUV::SendUDP(NET_BUFFER_INDEX *buf)
{
}

void NetSocketUV::ReceiveTCP()
{
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();

	FS_DATA_HANDLE fs_data = ((NetSocketUV*)net)->fs_data;
	fs_data.recv_buffer = recv_buffer;

	uv_fs_open(GetLoop(net), &fs_data, "out_h.264", O_WRONLY | O_CREAT | O_APPEND, 0666, onOpenFile);
	//std::filebuf fb;
	//fb.open("out_h.264", std::ios::out/* | std::ios::binary*/);
	//std::ostream out(&fb);

	//if (fb.is_open())
	//{
	//	std::cout << "writed " << received_bytes << "bytes if file" << std::endl;
	//	out.write((char*)recv_buffer->GetData(), received_bytes);
	//}
	//fb.close();
	
	FILE* proxy = nullptr;
#ifdef WIN32
	//system("RTSPProxyServerForClient.exe -d -c -%s");
	//proxy = _popen("RTSP.exe -d -c -%s", "r");
	//_pclose(proxy);
#else
//	//system("./RTSPProxyServerForClient -d -c -%s");
	proxy = popen("./RTSP -c -%s", "r");
	pclose(proxy);

#endif // WIN32

	/*NET_BUFFER_INDEX* index = net->PrepareMessage(10, received_bytes, recv_buffer->GetData());
	assert(index);
	SendTCP(index);
	uv_update_time(loop);*/
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
		//fclose(uvsocket->getFile());
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
	//uv_update_time(GetLoop(uvsocket->net));
	//free(req->write_buffer.base);
}

void onCloseFile(uv_fs_t* req)
{
	printf("exit");
	FS_DATA_HANDLE fs_data = *(FS_DATA_HANDLE*)req;
	fs_data.recv_buffer->Delete(1024);
	//free(req->bufs->base);
	uv_fs_req_cleanup(&fs_data.write_req);
}

void onOpenFile(uv_fs_t* req)
{
	int result = req->result;
	int r;
	
	if (result < 0) {
		printf("Error at opening file: %s\n", uv_strerror((int)req->result));
	}
	printf("Successfully opened file.\n");

	FS_DATA_HANDLE fs_data = *(FS_DATA_HANDLE*)req;
	NetBuffer* netBuff = fs_data.recv_buffer;
	unsigned len = netBuff->GetLength();
	char* data = (char*)netBuff->GetData();
	uv_buf_t wr_buf = uv_buf_init(data, len);

	uv_fs_req_cleanup(req);
	r = uv_fs_write(GetLoop(fs_data.recv_buffer->owner->net), &fs_data.write_req, result, &wr_buf, 1, 0, OnWriteFile);
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

void OnWriteFile(uv_fs_t* req)
{
	int result = req->result;
	int r;
	FS_DATA_HANDLE* fs_data = (FS_DATA_HANDLE*)req;
	NetBuffer* recvBuffer = fs_data->recv_buffer;
	NetSocketUV* sock = (NetSocketUV*)recvBuffer->owner->net->getReceivingSocket();
	if (result < 0) {
		printf("Error at writing file: %s\n", uv_strerror(result));
	}

	uv_fs_req_cleanup(req);
	r = uv_fs_close(GetLoop(sock->net), &fs_data->close_req, result, onCloseFile);
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

