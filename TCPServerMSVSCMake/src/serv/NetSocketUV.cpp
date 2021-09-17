#include "NetSocketUV.h"
#include "../../TCPServerMSVSCMake.h"

extern uv_loop_t* servloop;
extern uv_tcp_t* server;

NetSocketUV::NetSocketUV(Net* net)
{
}

NetSocketUV::~NetSocketUV()
{
}

bool NetSocketUV::Create(bool udp_tcp, int port, bool listen)
{
	NetSocketUV::Create(udp_tcp, port, listen);

	uv_loop_t* loop = GetLoop(sock);

	if (udp_tcp)
	{
		sock = malloc(sizeof(TCP_SOCKET));
		memset(sock, 0, sizeof(TCP_SOCKET));
		((TCP_SOCKET*)sock)->net_socket = this;
	}
	return false;
}

bool NetSocketUV::SetConnectedSocketToReadMode()
{
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
	return false;
}

bool NetSocketUV::Connect(sockaddr* addr)
{
	return false;
}

bool NetSocketUV::Accept()
{
	return false;
}

void NetSocketUV::SendTCP(char* buf)
{
}

void NetSocketUV::SendUDP(char* buf)
{
}

void NetSocketUV::RecciveTCP()
{
}

void NetSocketUV::ReciveUDP()
{
}

uv_tcp_t* NetSocketUV::GetPtrTCP(void* ptr) //����������� ����� ip 
{
	return nullptr;
}

uv_tcp_t* GetPtrTCP(void* ptr)
{
	return (uv_tcp_t*)(((char*)ptr) + sizeof(void*));
}

uv_loop_t* GetLoop(void* prt)
{
	return (uv_loop_t*)(((char*)prt) + sizeof(void*));
}

void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	NetSocket* socket = GetNetSocketPtr(stream);

	if (nread < 0) {
		if (nread == UV_EOF) {
			// end of file
			uv_close((uv_handle_t*)stream, OnCloseSocket);
		}
	}
	else if (nread > 0) {
		char* recv_buff;
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

void OnConnection(uv_connect_t* req, int status)
{
	if (status == 0) {
		fprintf(stderr, "Connection fail!\n", strerror(status));
		return;
	}
	/*
	else {

	}*/
}