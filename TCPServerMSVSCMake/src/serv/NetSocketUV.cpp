#include "NetSocketUV.hpp"

NetSocketUV::NetSocketUV(Net* net) : NetSocket(net)
{
	this->net = net;
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
	NetSocket::Create(udp_tcp, udp_tcp, listen);
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
				
				int i = uv_ip4_addr("127.0.0.1", 8000, sock_addres);
				assert(i == 0);
				int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
				assert(b == 0);
				int l = uv_listen((uv_stream_t*)tcp, 1024, OnAccept);
				if (l)
					return false;
				else
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

bool NetSocketUV::GetIP(Net_Address* addr, bool own_or_peer)
{
	if (own_or_peer)
	{
		//uv_tcp_t* tcp = GetPtrTCP(sock);
		std::cout << "Set IP to socket!" << std::endl;
		udp_tcp = true;

	}
	else
		return false;
}

bool NetSocketUV::Accept(uv_stream_t* handle)
{
	NetSocketUV* accept_sock = NewSocket(net);
	accept_sock->Create(8000, true, false);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);

	//SetID(client);

	if (uv_accept(handle, (uv_stream_t*)client) == 0)
	{
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP) == 0)
		{
			sockaddr sockname;
			int socklen = sizeof sockname;
			std::cout << "Start accepting RTSP from: " << uv_tcp_getsockname(client, &sockname, &socklen) << std::endl;
			return true;
		}
		else
			return false;
	}
	else
	{
		std::cout << "Cannot accepting client!" << std::endl;
		return false;
	}
}

//высылаем данные по TCP протаколу
void NetSocketUV::SendTCP(NET_BUFFER_INDEX *buf)
{
	if (buf->GetLength() > 0)
	{
		uv_buf_t buffer;
		buffer.len = buf->GetLength();
		buffer.base = (char*)&buf->GetData();
		int r = uv_write(((NetBufferUV*)buf)->GetPtrWrite(), (uv_stream_t*)GetPtrTCP(sock), &buffer, 1, OnWrite);
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
	//net->recv_buf.Add(received_bytes, (void*)&recv_buffer->GetData());

	//if (net->IsServer())
	//	net->ReciveMessege();
}

void NetSocketUV::ReceiveUPD()
{
}



//callback функция которая вызываемая uv_read_start - записывает полученные данные в массив данных
void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(stream);
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
		char* data = reinterpret_cast<char*>(recv_buff->GetData().data());
		assert(buf->base == data);
		uvsocket->net->recv_buf.Add(nread, (void*)buf);
		//recv_buff->SetMaxSize(nread);
		//uvsocket->ReceiveTCP();

		NET_BUFFER_LIST *list = uvsocket->net->GetSendList();
		list->SetOwner(uvsocket->net);
		NET_BUFFER_INDEX* bi = uvsocket->PrepareMessage(uvsocket->net->ClientID, recv_buff->GetLength(), reinterpret_cast<void*>(recv_buff->GetData().data()));
		
		uvsocket->SendMessenge(bi);
	}
}

//callback функция которая выделяет место под буффер данных
void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	std::cout << "Allocating buffer" << std::endl;
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = net_sock->net->GetRecvBuffer();

	unsigned int max_length = recv_buffer->GetLength();
	if (max_length < suggested_size)
		recv_buffer->SetMaxSize(suggested_size);
	unsigned char* databuff = reinterpret_cast<unsigned char*>(recv_buffer->GetData().data());
	buf->len = suggested_size;
	buf->base = reinterpret_cast<char*>(databuff);
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
	assert(buf->base == (char*)&recv_buffer->GetData());
	//recv_buffer->SetLength(nread);

	int r = uv_ip4_name((sockaddr_in*)addr, address_converter, sizeof(address_converter));
	socket->net->addr->address = address_converter;
	unsigned char* port_ptr = (unsigned char*)&(((sockaddr_in*)addr)->sin_port);
	socket->net->addr->port = port_ptr[1];
	socket->net->addr->port += port_ptr[0] << 8;

	socket->ReceiveUPD();
}

//callback функция вызываемая из Accept
void OnAccept(uv_stream_t* stream, int status)
{
	std::cout << "___ On Connetc ___" << std::endl;
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(stream);
	net_sock->Accept(stream);
}
//удаляем сокет libuv и приметив сокета
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

NET_BUFFER_INDEX* NetSocketUV::PrepareMessage(unsigned int sender_id, int length, void* data)
{
	int struct_size = sizeof(Send_Message);
	int len = length + struct_size;

	int len16 = len;

	NET_BUFFER_LIST* list = net->GetSendList();
	MEM_DATA buf;
	buf.data = nullptr;
	buf.length = len16;
	int index = list->AddBuffer(buf);
	NET_BUFFER_INDEX* bi = list->Get(index);
	//bi->SetLength(len);
	unsigned char* buf_data = (unsigned char*)bi->GetData().data();
	Send_Message* sm = (Send_Message*)buf_data;

	//sm->sender = sender_id;
	sm->len = length;
	if (length)
		memcpy(&(buf_data[struct_size]), &data, length);

	return bi;
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
/*
bool NetSocketUV::SetConnectedSocketToReadMode(uv_stream_t *stream)
{
	if (udp_tcp)
	{
		std::cout << "Setting connected UV socket to reading mode!" << std::endl;
		int r = uv_read_start(stream, OnAllocBuffer, OnReadTCP);
		if (r == 0)
			return true;
		else
			return false;

	}
	return false;
}
*/