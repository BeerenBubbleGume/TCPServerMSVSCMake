#include "NetSocketUV.hpp"

NetSocketUV::NetSocketUV(Net *_Net) : NetSocket(net)
{
	//this->net = net;
	sock = NULL;
	status = errno;	

	loop = uv_default_loop();
	uv_loop_init(loop);
	server = new uv_tcp_t;
	client = new uv_tcp_t;
	udp = new uv_udp_t;
}

NetSocketUV::~NetSocketUV()
{
	status = 0;
	//free(sock);
	//free(server);
	//free(client);
	//delete[] net;
}

bool NetSocketUV::Create(Net_Address* addr, int port, bool udp_tcp)
{
	NetSocket::Create(addr, udp_tcp, listen);
	uv_loop_t* mloop = GetLoop(net);
	if (mloop)
	{
		std::cout << "Loop have been initialize!" << std::endl;
		if (udp_tcp)
		{
			sock = malloc(sizeof(TCP_SOCKET));
			memset(sock, 0, sizeof(TCP_SOCKET));
			((TCP_SOCKET*)sock)->net_socket = this;


			std::cout << "Create socket!" << std::endl;
			//assert(net->CreateSocket() == true);

			int r = uv_tcp_init(mloop, GetPtrServerTCP(sock));
			assert(r == 0);

			r = uv_tcp_nodelay(GetPtrServerTCP(sock), true);
			assert(r == 0);
			
			sockaddr_in* sock_addres = new sockaddr_in;
			int i = uv_ip4_addr(addr->address, addr->port, sock_addres);
			assert(i == 0);
			int b = uv_tcp_bind(GetPtrServerTCP(sock), (sockaddr*)sock_addres, 0);
			assert(b == 0);

			int listen = uv_listen((uv_stream_t*)GetPtrServerTCP(sock), 1024, OnConnect);
			if (listen == 0)
			{
				if (GetIP(addr, true) == true)
					return true;
				else
					return false;
			}			
		}
		else
		{
			sock = malloc(sizeof(UDP_SOCKET));
			memset(sock, 0, sizeof(UDP_SOCKET));
			int r = uv_udp_init(loop, udp);
			assert(r == 0);

			struct sockaddr_in broadcast_addr;
			uv_ip4_addr(addr->address, addr->port, &broadcast_addr);
			r = uv_udp_bind(udp, (const struct sockaddr*)&broadcast_addr, 0);
			assert(r == 0);

			r = uv_udp_set_broadcast(udp, 1);
			assert(r == 0);

			if (listen)
			{
				r = uv_udp_recv_start(udp, OnAllocBuffer, OnReadUDP);
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

bool NetSocketUV::GetIP(Net_Address* addr, bool own_or_peer)
{
	if (own_or_peer)
	{
		//uv_tcp_t* tcp = GetPtrTCP(sock);
		std::cout << "Set IP to socket!" << std::endl;
		udp_tcp = true;

		uv_loop_t* sloop = GetLoop(net);
		RunLoop(sloop);

		return true;
	}
	else
		return false;
}
/*
bool NetSocketUV::ConnectUV(Net_Address* addr)
{
	sockaddr_in dest;
	uv_ip4_addr(addr->address, addr->port, &dest);

	uv_tcp_t* tcp = GetPtrTCP(sock);
	UV_CONNECT_DATA* connect_data = &((NetSocket*)net)->connect_data;
	connect_data->net = net;
	if (!uv_tcp_connect(connect_data, tcp, (const struct sockaddr*)&dest, OnConnect))
	{
		uv_loop_t* loop = GetLoop(net);
		uv_run(loop, UV_RUN_DEFAULT);
		return true;
	}
	return false;
}
*/
bool NetSocketUV::Accept()
{
	uv_tcp_t* server = GetPtrServerTCP(sock);
	uv_tcp_t* client = GetPtrClientTCP(sock);
	if (uv_accept((uv_stream_t*)server, (uv_stream_t*)client) == 0)
	{
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP))
		{
			std::cout << "Start accepting RTSP!" << std::endl;
			return true;
		}
		else
		{
			std::cout << "Cannot start reading!" << std::endl;
			return false;
		}
	}
}

//высылаем данные по TCP протаколу
void NetSocketUV::SendTCP(NET_BUFFER_INDEX *buf)
{
	if (buf->buff_length > 0)
	{
		uv_buf_t buffer;
		buffer.len = buf->buff_length;
		buffer.base = (char*)buf->DataBuff;
		int r = uv_write(((NetBufferUV*)buf)->GetPtrWrite(), (uv_stream_t*)server, &buffer, 1, OnWrite);
		assert(r == 0);
	}
}

void NetSocketUV::SendUDP(NET_BUFFER_INDEX *buf)
{
}

void NetSocketUV::ReceiveTCP()
{
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	net->recv_buf->Add(received_bytes, recv_buffer->GetData());

	if (net->IsServer())
		net->ReciveMessege();
}

void NetSocketUV::ReceiveUPD()
{
}

bool NetSocketUV::SetConnectedSocketToReadMode()
{
	/*if (udp_tcp)
	{
		std::cout << "Setting connected UV socket to reading mode!" << std::endl;
		int r = uv_read_start(stream, OnAllocBuffer, OnReadTCP);
		if (r == 0)
			return true;
		else
			return false;

	}*/
	return false;
}

//callback функция которая вызываемая uv_read_start - записывает полученные данные в массив данных
void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(stream);
	std::cout << "Receiving..." << std::endl;
	if (nread < 0)
	{
		std::cout << "read buff < 0" << std::endl;
		uvsocket->net->OnLostConnection(socket);
	}
	else
	{
		std::cout << "Reading UV socket" << std::endl;
		NetBuffer* recv_buff = uvsocket->net->GetRecvBuffer();
		assert(buf->base == (char*)recv_buff->DataBuff);
		recv_buff->SetMaxSize(nread);
		uvsocket->ReceiveTCP();
		uvsocket->SendTCP((NET_BUFFER_INDEX*)recv_buff->GetData());
	}
}

//callback функция которая выделяет место под буффер данных
void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	std::cout << "Allocating buffer" << std::endl;
	NetSocketUV* nsock = (NetSocketUV*)GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = nsock->net->GetRecvBuffer();

	unsigned int max_length = 0;
	if (max_length < suggested_size)
		recv_buffer->SetMaxSize(suggested_size);

	buf->len = suggested_size;
	buf->base = (char*)recv_buffer->GetData();
}

//функция закрывающая libuv сокет
void OnCloseSocket(uv_handle_t *handle)
{
	free(handle);
}

void OnWrite(uv_write_t *req, int status)
{
	int offset = offsetof(NetBufferUV, sender_object);
	NetBufferUV* buf = (NetBufferUV*)(((char*)req) - offset);
	NET_BUFFER_LIST* list = (NET_BUFFER_LIST*)buf->owner;
	int index = buf->GetIndex();
	
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

void NetSocketUV::RunLoop(uv_loop_t *loop)
{
	if (loop)
	{
		std::cout << "Run loop!" << std::endl;
		uv_run((uv_loop_t *)loop, UV_RUN_DEFAULT);
	}
	else
	{
		fprintf(stderr, "Cannot run loop");
		exit(4);
	}
}

//callback функция вызываемая из Accept
void OnConnect(uv_stream_t* stream, int status)
{
	std::cout << "___ On Connetc ___" << std::endl;
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	NetSocketUV* netsock = (NetSocketUV*)GetNetSocketPtr(stream);
	netsock->Accept();
}
//удаляем сокет libuv и приметив сокета
void NetSocketUV::Destroy()
{
	if (sock)
	{
		if (udp_tcp)
		{
			uv_close((uv_handle_t *)server, OnCloseSocket);
			((TCP_SOCKET *)sock)->net_socket = nullptr;
		}
		else
		{
			int r = uv_read_stop((uv_stream_t *)udp);
			assert(r == 0);
			uv_close((uv_handle_t *)sock, OnCloseSocket);
			((UDP_SOCKET *)sock)->net_socket = NULL;
		}
		sock = NULL;
	}
	NetSocket::Destroy();
}

uv_tcp_t *GetPtrTCP(void *ptr)
{
	return (uv_tcp_t*)(((char*)ptr) + sizeof(void*));
}

uv_tcp_t* GetPtrServerTCP(void* ptr)
{
	NetSocketUV* client = (NetSocketUV*)GetNetSocketPtr(ptr);
	return client->server;
}

uv_tcp_t* GetPtrClientTCP(void* ptr)
{
	NetSocketUV* serv = (NetSocketUV*)GetNetSocketPtr(ptr);
	return serv->client;
}

uv_udp_t *GetPtrUDP(void *ptr)
{
	return (uv_udp_t *)(((char *)ptr) + sizeof(void *));
}

uv_loop_t *GetLoop(Net* net)
{
	NetSocketUV* serv = (NetSocketUV*)(net);
	if (net != nullptr)
	{
		uv_loop_t* theloop;
		theloop = uv_default_loop();
		//int r = uv_loop_init(serv->loop);
		//assert(r == 0);
		return theloop;
	}
	else
		return nullptr;
}



/*
void OnConnect(uv_connect_t* req, int status)
{
	Net* net = ((UV_CONNECT_DATA*)req)->net;
	if (status == 0)
	{
		net->OnConnect(true);
	}
	else
	{
		//ErrorLibUV(status);
		net->OnConnect(false);
	}
}
*/