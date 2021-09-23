/* NetSocket дочерний класс NetSocket используещий событийный цикл из библиотеки libuv
* для преобразования абстракций используются методы из NetSocket (GetPtrSocket, GetNetSocketPtr) и методы: GetPtrTCP, GetLoop, GetPtrUDP
* для корректного запуска требуется в классе сервера вызвать метод GetIP, после чего по цепочки вызывутся все включенные методы класса NetSocketUV
*/

#include "NetSocketUV.h"
#include <cassert>
#include <fstream>
#include <cstring>
#pragma comment(lib, "uv.lib")

bool udp_tcp;

NetSocketUV::NetSocketUV()
{
	net = new NetBuffer;
}

NetSocketUV::NetSocketUV(Net* net)
{
}

NetSocketUV::~NetSocketUV()
{
}

bool NetSocketUV::Create(const char* ip, bool udp_tcp, int port, bool listen)
{
	//NetSocketUV::Create(udp_tcp, port, listen);

	uv_loop_t* loop = GetLoop(sock);
	uv_tcp_t* server = GetPtrTCP(sock);
	uv_ip4_addr(ip, 8000, net_addr);
	if (udp_tcp)
	{
		sock = malloc(sizeof(TCP_SOCKET));
		memset(sock, 0, sizeof(TCP_SOCKET));
		((TCP_SOCKET*)sock)->net_socket = this;
		GetIP(net_addr, true);
	}
	uv_tcp_init(loop, server);

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

//получаем IP адресс 
bool NetSocketUV::GetIP(sockaddr_in* addr, bool own_or_peer)
{
	uv_tcp_t* server = GetPtrTCP(sock);
	net_addr = addr;

	if (own_or_peer) 
	{
		int gip = uv_tcp_bind(server, (sockaddr*)net_addr, 0);
		if (gip)
		{
			NetSocketUV::Connect((sockaddr*)net_addr);
		}
		return true;
	}
	return false;
}

//настраиваем соединение
bool NetSocketUV::Connect(sockaddr* addr)
{
	uv_tcp_t* server = GetPtrTCP(sock);

	if (addr)
	{
		uv_connect_t* req = new uv_connect_t;
		memset(req, 0, sizeof(TCP_SOCKET));
		uv_tcp_connect(req, server, addr, OnConnect);
	}

	return false;
}


bool NetSocketUV::Accept()
{
	uv_tcp_t* server = GetPtrTCP(sock);
	uv_loop_t* loop = GetLoop(sock);

	if (udp_tcp)
	{
		uv_stream_t* client = (uv_stream_t*)malloc(sizeof(sock));
		int s = uv_accept((uv_stream_t*)server, client);
		SetID(client);
		return (s == 0);
		std::cout << "Accepted!" << std::endl;
	}
	
	return uv_run(loop, UV_RUN_DEFAULT);
}

//высылаем данные по TCP протаколу
void NetSocketUV::SendTCP(char* buf)
{
	if (udp_tcp)
	{
		const uv_buf_t* uv_buf = (uv_buf_t*)buf;
		uv_write_t* wr = new uv_write_t;
		memset(wr, 0, sizeof(uv_write_t*));
		uv_stream_t* client = new uv_stream_t;
		uv_write(wr, client, uv_buf, 4096, OnWrite);
	}
	else 
	{
		fprintf(stderr, "Nothing to send!\n");
		return;
	}

}

void NetSocketUV::SendUDP(char* buf)
{
}

//принимаем данные по TCP протоколу
void NetSocketUV::ReciveTCP()
{
	uv_stream_t* server = (uv_stream_t*)GetPtrTCP(sock);
	int list = uv_listen(server, 1024, OnAccept);
	uv_stream_t* client = (uv_stream_t*)GetPtrTCP(sock);

	if (list)
	{
		udp_tcp = true;
	}
	else
	{
		udp_tcp = false;
		uv_close((uv_handle_t*)client, OnCloseSocket);
	}
}

void NetSocketUV::ReciveUDP()
{
}

//удаляем сокет libuv и приметив сокета
void NetSocketUV::Destroy()
{
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

//callback функция которая вызываемая uv_read_start - записывает полученные данные в массив данных
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	NetSocketUV* socket = (NetSocketUV*)GetNetSocketPtr(stream);

	if (nread < 0) {
		if (nread == UV_EOF) {
			// end of file
			uv_close((uv_handle_t*)stream, OnCloseSocket);
		}
	}
	else if (nread > 0) {
		NetBuffer* recv_buff = socket->net->GetReciveBuffer();
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


	NetSocketUV* netsock = (NetSocketUV*)GetNetSocketPtr(stream);
	uv_loop_t* servloop = GetLoop(netsock);

	uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(servloop, client);
	if (uv_accept(stream, (uv_stream_t*)client) == 0) // �������� ������� ���������� ����������
	{
		netsock->SetConnectedSocketToReadMode();
	}
	else
	{
		// ���������� ���������� �� �������
		uv_close((uv_handle_t*)client, OnCloseSocket); // ������� �����, ������� ��� ������ ��� ����������
	}
}


//callback функция которая выделяет место под буффер данных
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	*buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
	buf->base = (char*)suggested_size;
}

//функция закрывающая libuv сокет
void OnCloseSocket(uv_handle_t* handle) {
	free(handle);
}

void OnWrite(uv_write_t* req, int status)
{
	if (status != 0)
	{
		fprintf(stderr, "Sending TCP/UDP is fail!\n", uv_strerror(status));
	}
}

//callback функция вызываемая из Connect 
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
		NetSocketUV* netsock = (NetSocketUV*)GetNetSocketPtr(req);
		netsock->Accept();
	}

}
