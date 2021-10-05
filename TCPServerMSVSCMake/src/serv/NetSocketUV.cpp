/* NetSocket дочерний класс NetSocket используещий событийный цикл из библиотеки libuv
* для преобразования абстракций используются методы из NetSocket (GetPtrSocket, GetNetSocketPtr) и методы: GetPtrTCP, GetLoop, GetPtrUDP
* для корректного запуска требуется в классе сервера вызвать метод GetIP, после чего по цепочки вызывутся все включенные методы класса NetSocketUV
*/

#include "NetSocketUV.hpp"


//bool udp_tcp;
//static uv_loop_t *loop = uv_default_loop();
//static uv_tcp_t *server = new uv_tcp_t;

NetSocketUV::NetSocketUV(Net *_Net) : NetSocket(net)
{
	this->net = net;
	sock = NULL;
	status = errno;	
}

NetSocketUV::~NetSocketUV()
{
	status = 0;
	free(sock);
	//delete[] net;
}

bool NetSocketUV::Create(Net_Address* addr, bool udp_tcp,bool listen)
{
	//NetSocketUV::Create(ip, udp_tcp, port, listen);
	uv_loop_t* loop = GetLoop(net);
	assert(loop != nullptr);
	if (udp_tcp)
	{
		sock = malloc(sizeof(TCP_SOCKET));
		//memset(sock, 0, sizeof(TCP_SOCKET));
		((TCP_SOCKET *)sock)->net_socket = this;

		
		std::cout << "Create socket!" << std::endl;
		assert(CreateSocket(sock, net_addr) == true);

		int r = uv_tcp_init(loop, GetPtrTCP(sock));
		assert(r == 0);

		r = uv_tcp_nodelay(GetPtrTCP(sock), true);
		assert(r == 0);

		uv_tcp_t* tcp = GetPtrTCP(sock);
		if (listen)
		{
			sockaddr_in* sock_addres = new sockaddr_in;
			uv_ip4_addr(addr->address, addr->port, sock_addres);
			int b = uv_tcp_bind(tcp, (sockaddr*)addr, 0);
			assert(b == 0);
			if (GetIP(addr, true) == true)
				return false;
		}
		
	}
	else
	{
		sock = malloc(sizeof(UDP_SOCKET));
		memset(sock, 0, sizeof(UDP_SOCKET));
		uv_udp_t* udp = GetPtrUDP(sock);
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

bool NetSocketUV::GetIP(Net_Address* addr, bool own_or_peer)
{
	if (own_or_peer)
	{
		uv_tcp_t* tcp = GetPtrTCP(sock);
		std::cout << "Set IP to socket!" << std::endl;
		int r = uv_listen((uv_stream_t*)tcp, 1024, OnConnect);;
		if (r == 0)
		{
			uv_loop_t* loop = GetLoop(net);
			RunLoop(loop);
			return true;
		}

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
	if (udp_tcp)
	{
		int list;
		
		uv_tcp_t* socket = GetPtrTCP(net);

		if (list = uv_listen((uv_stream_t*)socket, 1024, OnConnect)  == 0)
		{
			std::cout << "Start listening UV socket!" << std::endl;
			return true;
		}
		else
		{
			fprintf(stderr, "Cannot listen UV socket!\n", uv_err_name(list));
			exit(2);
			return false;
		}
	}
}

//высылаем данные по TCP протаколу
void NetSocketUV::SendTCP(NET_BUFFER_INDEX *buf)
{
	if (buf->length > 0)
	{
		uv_buf_t buffer;
		buffer.len = buf->length;
		buffer.base = (char*)buf->DataBuff;
		int r = uv_write(((NetBufferUV*)buf)->GetPtrWrite(), (uv_stream_t*)GetPtrTCP(sock), &buffer, 1, OnWrite);
		assert(r == 0);
	}
}

void NetSocketUV::SendUDP(NET_BUFFER_INDEX *buf)
{
}

void NetSocketUV::ReceiveTCP()
{
	NetBuffer* recv_buffer = GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	recv_buf.Add(received_bytes, recv_buffer->GetData());

	if (net->IsServer())
		Recive();
}

void NetSocketUV::ReceiveUPD()
{
}

bool NetSocketUV::SetConnectedSocketToReadMode(uv_stream_t *stream)
{
	if (udp_tcp)
	{
		std::cout << "Setting connected UV socket to reading mode!" << std::endl;
		uv_tcp_t* tcp = GetPtrTCP(stream);
		int r = uv_read_start((uv_stream_t*)tcp, OnAllocBuffer, OnReadTCP);
		return (r == 0);
	}
	return false;
}

//callback функция которая вызываемая uv_read_start - записывает полученные данные в массив данных
void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{

	NetSocketUV* socket = (NetSocketUV*)GetNetSocketPtr(stream);
	
	if (nread < 0)
	{
		if (nread == UV_EOF)
		{
			std::cout << "read buff < 0" << std::endl;
			socket->net->OnLostConnection(socket);
		}
	}
	else
	{
		std::cout << "Reading UV socket" << std::endl;
		NetBuffer* recv_buff = socket->net->GetRecvBuffer();
		assert(buf->base == (char*)recv_buff->GetData());
		recv_buff->SetLength(nread);
		socket->ReceiveTCP();
		//NET_BUFFER_INDEX* buf = (NET_BUFFER_INDEX*)recv_buff->GetData();
		//socket->SendTCP(buf);
	}
}

//callback функция которая выделяет место под буффер данных
void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	NetSocket* socket = GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = new NetBuffer;

	unsigned int max_length = recv_buffer->GetMaxLength();
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
	NetBuffer* recv_buffer = socket->GetRecvBuffer();
	assert(buf->base == (char*)recv_buffer->GetData());
	recv_buffer->SetLength(nread);

	int r = uv_ip4_name((sockaddr_in*)addr, address_converter, sizeof(address_converter));
	socket->addr->address = address_converter;
	unsigned char* port_ptr = (unsigned char*)&(((sockaddr_in*)addr)->sin_port);
	socket->addr->port = port_ptr[1];
	socket->addr->port += port_ptr[0] << 8;

	socket->ReceiveUPD();
}

void NetSocketUV::RunLoop(uv_loop_t *loop)
{
	if (loop)
	{
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
	uv_loop_t* loop = GetLoop(netsock);
	uv_tcp_t* client = new uv_tcp_t;
	memset(client, 0, sizeof(uv_tcp_t*));
	uv_tcp_init(loop, client);
	int r = uv_accept(stream, (uv_stream_t*)client);
	if (r)
	{
		// ���������� ���������� �� �������
		uv_close((uv_handle_t *)client, OnCloseSocket); // ������� �����, ������� ��� ������ ��� ����������
		exit(4);
	}
	std::cout << "Net connetction" << std::endl;
	bool c = netsock->SetConnectedSocketToReadMode((uv_stream_t*)client) == true;
	assert(c == true);
}
//удаляем сокет libuv и приметив сокета
void NetSocketUV::Destroy()
{
	if (sock)
	{
		if (udp_tcp)
		{
			uv_tcp_t *tcp = GetPtrTCP(sock);
			uv_close((uv_handle_t *)tcp, OnCloseSocket);
			((TCP_SOCKET *)sock)->net_socket = nullptr;
		}
		else
		{
			uv_udp_t *udp = GetPtrUDP(sock);
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

uv_udp_t *GetPtrUDP(void *ptr)
{
	return (uv_udp_t *)(((char *)ptr) + sizeof(void *));
}

uv_stream_t *GetPtrStream(void *ptr)
{
	return (uv_stream_t *)(((char *)ptr) + sizeof(void *));
}

NetBuffer *GetPtrBuffer(void *ptr)
{
	return (NetBuffer *)(((char *)ptr) + sizeof(void *));
}

uv_loop_t *GetLoop(Net* net)
{
	net = new Net;
	NetSocketUV* serv = (NetSocketUV*)(net);
	if (net != nullptr)
	{
		serv->theloop = uv_default_loop();
		int r = uv_loop_init(serv->theloop);
		assert(r == 0);
		return serv->theloop;
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