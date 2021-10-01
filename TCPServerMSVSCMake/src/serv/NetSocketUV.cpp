/* NetSocket дочерний класс NetSocket используещий событийный цикл из библиотеки libuv
* для преобразования абстракций используются методы из NetSocket (GetPtrSocket, GetNetSocketPtr) и методы: GetPtrTCP, GetLoop, GetPtrUDP
* для корректного запуска требуется в классе сервера вызвать метод GetIP, после чего по цепочки вызывутся все включенные методы класса NetSocketUV
*/

#include "NetSocketUV.hpp"


//bool udp_tcp;
static uv_loop_t *loop = uv_default_loop();
static uv_tcp_t *server = new uv_tcp_t;

NetSocketUV::NetSocketUV(Net *_Net) : NetSocket()
{
	sock = NULL;
	status = errno;
	net = (NetSocket*)malloc((sizeof(NetSocket)));
}

NetSocketUV::NetSocketUV()
{
	sock = NULL;
	status = errno;
	net = (NetSocket*)malloc((sizeof(NetSocket)));
}

NetSocketUV::~NetSocketUV()
{
	status = 0;
	free(sock);
	//delete[] net;
}

bool NetSocketUV::Create(const char *ip, bool udp_tcp, int port, bool listen)
{
	//NetSocketUV::Create(ip, udp_tcp, port, listen);
	if (udp_tcp)
	{
		sock = malloc(sizeof(TCP_SOCKET));
		memset(sock, 0, sizeof(TCP_SOCKET));
		((TCP_SOCKET *)sock)->net_socket = this;

		loop = uv_default_loop();
		uv_loop_init(loop);
		std::cout << "Create socket!" << std::endl;
		assert(CreateSocket(sock, net_addr) == true);
		std::cout << "UV socket created success!" << std::endl;
		int g = GetIP(ip, true);
		assert(g == 0);
		
	}
	else
	{
		sock = malloc(sizeof(UDP_SOCKET));
		memset(sock, 0, sizeof(UDP_SOCKET));
		uv_udp_t* udp = GetPtrUDP(sock);
		int r = uv_udp_init(loop, udp);
		assert(r == 0);

		struct sockaddr_in broadcast_addr;
		uv_ip4_addr("0.0.0.0", port, &broadcast_addr);
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
	RunLoop(loop);
	return false;
}

bool NetSocketUV::GetIP(const char *ip, bool own_or_peer)
{
	std::cout << "GetIP" << std::endl;
	sockaddr_in *addr = new sockaddr_in;
	if (own_or_peer)
	{
		std::cout << "Set IP to socket!" << std::endl;
		assert(uv_ip4_addr(ip, 8000, addr) == 0);
		if(ConnectUV(8000, ip, addr) == true)
			return true;
	}
	else
		return false;
}

bool NetSocketUV::ConnectUV(int port, const char *ip, sockaddr_in *addr)
{
	if ((loop && server) != NULL)
	{
		assert(uv_tcp_init(loop, server) == 0);
		int gip = uv_tcp_bind(server, (sockaddr *)addr, 0);
		if(gip == 0)
			std::cout << "Binding UV socket!" << std::endl;
		udp_tcp = true;

		if (Accept() == true)
			std::cout << "====Accepting UV socket====" << std::endl;
	}

	return false;
}

bool NetSocketUV::Accept()
{
	if (udp_tcp)
	{
		int list;
		
		if (list = uv_listen((uv_stream_t*)server, 1024, OnConnect)  == 0)
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
	NetBuffer* recv_buffer = GetReciveBuffer();
	int received_bytes = recv_buffer->GetLength();
	rbuffer.Add(received_bytes, recv_buffer->GetData());

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
		int r = uv_read_start(stream, OnAllocBuffer, OnReadTCP);
		assert(r == 0);
		return true;
	}
	return false;
}

//callback функция которая вызываемая uv_read_start - записывает полученные данные в массив данных
void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{

	NetSocketUV* socket = (NetSocketUV*)malloc(sizeof(NetSocketUV));
	
	if (nread < 0)
	{
		if (nread == UV_EOF)
		{
			socket->net->OnLostConnection(socket);
		}
	}
	else
	{
		std::cout << "Reading UV socket" << std::endl;
		NetBuffer* recv_buff = new NetBuffer;
		assert(buf->base == (char*)recv_buff->GetData());
		recv_buff->SetLength(nread);
		socket->ReceiveTCP();
	}
}

//callback функция которая выделяет место под буффер данных
void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	NetSocket* socket = (NetSocket*)malloc((sizeof(NetSocket*)));
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
	NetBuffer* recv_buffer = socket->GetReciveBuffer();
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

	NetSocketUV netsock;

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
	netsock.SetConnectedSocketToReadMode((uv_stream_t*)client) == true;
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
	return (uv_tcp_t *)(((char *)ptr) + sizeof(void *));
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

uv_loop_t *GetLoop(void *prt)
{
	return (uv_loop_t *)(((char *)prt) + sizeof(void *));
}