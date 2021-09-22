#include "NetSocketUV.h"
#include <cassert>
#include <fstream>
#include <cstring>

uv_loop_t* servloop;
uv_tcp_t* server;
bool udp_tcp;
//FILE* file = fopen("ErrorLog.txt", "w");

NetSocketUV::NetSocketUV(Net* net)
{
}

NetSocketUV::~NetSocketUV()
{
}

bool NetSocketUV::Create(bool udp_tcp, int port, bool listen)
{
	//NetSocketUV::Create(udp_tcp, port, listen);

	uv_loop_t* loop = GetLoop(sock);

	if (udp_tcp)
	{
		sock = malloc(sizeof(TCP_SOCKET));
		memset(sock, 0, sizeof(TCP_SOCKET));
		((TCP_SOCKET*)sock)->net_socket = this;
	}
	NetSocketUV::SetConnectedSocketToReadMode();
	return false;
}

bool NetSocketUV::SetConnectedSocketToReadMode()
{
	std::cout << "netsockuv SetConnectedSocketToReadMode" << std::endl;
	if (udp_tcp)
	{
		uv_tcp_t* tcp = GetPtrTCP(sock);
		int r = uv_read_start((uv_stream_t*)tcp, OnAllocBuffer, OnReadTCP);
		return (r == 0);
	}
	return false;
}

bool NetSocketUV::GetIP(sockaddr_in* addr, bool own_or_peer)
{
	std::cout << "netsockuv GetIP" << std::endl;
	if (own_or_peer) {
		uv_tcp_init(servloop, server);
		uv_ip4_addr("0.0.0.0", 544, addr);
		int gip = uv_tcp_bind(server, (sockaddr*)addr, 0);
		
		return NetSocketUV::Connect((sockaddr*)addr);
	}
	return false;
}

bool NetSocketUV::Connect(sockaddr* addr)
{
	std::cout << "netsockuv Connect" << std::endl;
	if (addr)
	{
		uv_connect_t* req = (uv_connect_t*)malloc(sizeof(uv_connect_t*));
		memset(req, 0, sizeof(TCP_SOCKET));
		uv_tcp_connect(req, server, addr, OnConnect);
	}
	return true;
}

bool NetSocketUV::Accept()
{
	std::cout << "netsockuv Accept" << std::endl;
	if (udp_tcp)
	{
		uv_stream_t* client = (uv_stream_t*)malloc(sizeof(sock));
		int s = uv_accept((uv_stream_t*)server, client);
		if(s)
			
		return (s == 0);
	}
	return false;
}

void NetSocketUV::SendTCP(char* buf)
{
	std::cout << "netsockuv SendTCP" << std::endl;
	if (udp_tcp)
	{
		const uv_buf_t* uv_buf = (uv_buf_t*)buf;
		uv_write_t* wr;
		memset(wr, 0, sizeof(uv_write_t*));
		uv_stream_t* client = (uv_stream_t*)malloc(sizeof(uv_stream_t*));
		uv_write(wr, client, uv_buf, 4096, OnWrite);
	}
	else
	{
		
	}

}

void NetSocketUV::SendUDP(char* buf)
{
}

void NetSocketUV::RecciveTCP()
{
	printf("netsockuv recivetcp");
	int list = uv_listen((uv_stream_t*)server, 1024, OnConnection);
	if (list < 0)
	{
		fprintf(stderr, "RecivingTCP is fail!\n", uv_strerror(-1));
		exit(1);
	}
}

void NetSocketUV::ReciveUDP()
{
}

void NetSocketUV::Destroy()
{
	printf("netsockuv destroy");
	if (sock)
	{
		if (udp_tcp)
		{
			uv_tcp_t* tcp = GetPtrTCP(sock);
			uv_close((uv_handle_t*)tcp, OnCloseSocket);
			((TCP_SOCKET*)sock)->net_socket = NULL;
		}
		else {
			uv_udp_t* udp = GetPtrUDP(sock);
			int r = uv_read_stop((uv_stream_t*)udp);
			assert(r == 0);
			uv_close((uv_handle_t*)sock, OnCloseSocket);
			((UDP_SOCKET*)sock)->net_socket = NULL;
		}
		sock = NULL;
	}
	NetSocket::Destroy();
}

uv_tcp_t* GetPtrTCP(void* ptr)
{
	return (uv_tcp_t*)(((char*)ptr) + sizeof(void*));
}

uv_udp_t* GetPtrUDP(void* ptr)
{
	return (uv_udp_t*)(((char*)ptr) + sizeof(void*));
}

uv_loop_t* GetLoop(void* prt)
{
	return (uv_loop_t*)(((char*)prt) + sizeof(void*));
}

void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	NetSocketUV* socket = (NetSocketUV*)GetNetSocketPtr(stream);

	if (nread < 0) {
		if (nread == UV_EOF) {
			// end of file
			uv_close((uv_handle_t*)stream, OnCloseSocket);
		}
	}
	else if (nread > 0) {
		NetBuffer* recv_buff = socket->net->GetReciveData();
		assert(buf->base == (char*)recv_buff->GetData());
		recv_buff->SetLength(nread);
		socket->ReciveTCP();
	}

	if (buf->base)
		free(buf->base);
}

void OnAccept(uv_stream_t* stream, int status)
{
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	// ������ ������� ����� �����, ����� ����� ���� ���������� ���������� � ��������

	uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(servloop, client);
	if (uv_accept(stream, (uv_stream_t*)client) == 0) // �������� ������� ���������� ����������
	{
		// ���������� �����������
		uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP); // ��������� ����� � ����� ������
	}
	else
	{
		// ���������� ���������� �� �������
		uv_close((uv_handle_t*)client, OnCloseSocket); // ������� �����, ������� ��� ������ ��� ����������
	}
}

void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	*buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
	buf->base = (char*)suggested_size;
}

void OnCloseSocket(uv_handle_t* handle) {
	free(handle);
}

void OnWrite(uv_write_t* req, int status)
{
	if (status == 0)
	{
		fprintf(stderr, "Sending TCP/UDP is fail!\n", uv_strerror(status));
	}
}

void OnConnection(uv_stream_t* req, int status)
{
	if (status == 0) {
		fprintf(stderr, "Connection fail!\n", uv_strerror(status));
		return;
	}
	
	else {
		
	}
}

void OnConnect(uv_connect_t* req, int status)
{
	if (status <= 0)
	{
		udp_tcp = false;
		fprintf(stderr, "Connection fali!\n", uv_strerror(status));
		return;
	}
	else
	{
		udp_tcp = true;
		NetSocketUV* sock = (NetSocketUV*)malloc(sizeof(TCP_SOCKET));
		sock->Accept();
	}

}
