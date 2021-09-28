/* NetSocket дочерний класс NetSocket используещий событийный цикл из библиотеки libuv
* для преобразования абстракций используются методы из NetSocket (GetPtrSocket, GetNetSocketPtr) и методы: GetPtrTCP, GetLoop, GetPtrUDP
* для корректного запуска требуется в классе сервера вызвать метод GetIP, после чего по цепочки вызывутся все включенные методы класса NetSocketUV
*/

#include "NetSocketUV.h"
#include <cassert>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <inttypes.h>
#include "Net.h"
#include "ServerUV.h"

bool udp_tcp;
static uv_loop_t *loop = uv_default_loop();
static uv_tcp_t *server = new uv_tcp_t;

NetSocketUV::NetSocketUV()
{
	status = errno;
	net = new Net;
	_net = new NetBuffer;
}

NetSocketUV::NetSocketUV(Net *_Net)
{
	_Net = new Net;
	//*((SOCKET *)sock) = tcp_socket;
}

NetSocketUV::~NetSocketUV()
{
	free(net);
	free(_net);
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
		GetIP(ip, true);
	}
	RunLoop(loop);
	return false;
}

bool NetSocketUV::GetIP(const char *ip, bool own_or_peer)
{
	sockaddr_in *addr = new sockaddr_in;
	if (own_or_peer)
	{
		std::cout << "Set IP to socket!" << std::endl;
		assert(0 == uv_ip4_addr(ip, 8000, addr));
		ConnectUV(8000, ip, addr);
		return true;
	}
	return false;
}

bool NetSocketUV::ConnectUV(int port, const char *ip, sockaddr_in *addr)
{
	if ((loop && server) != NULL)
	{
		assert(uv_tcp_init(loop, server) == 0);
		int gip = uv_tcp_bind(server, (sockaddr *)addr, 0);
		assert(gip == 0);
		std::cout << "Binding socket!" << std::endl;
		udp_tcp = true;
#ifdef WIN32
		Connect(addr, (SOCKET)sock);
#else		
		Connetct(addr, (intptr_t)sock);
#endif // WIN32

		assert(Accept() == true);
	}

	return false;
}

bool NetSocketUV::Accept()
{
	if (udp_tcp)
	{
		int list = uv_listen((uv_stream_t *)server, 1024, OnConnect);
		
		if (list == true)
		{
			fprintf(stderr, "Cannot listen socket!\n", uv_err_name(list));
			exit(2);
			return false;
		}
		else
		{
			std::cout << "Start listening socket!" << std::endl;
			return true;
		}
	}
}

//высылаем данные по TCP протаколу
void NetSocketUV::SendTCP(char *buf)
{
	Net *net = new Net;
#ifdef WIN32

	Send(buf, sizeof(buf));
#else
	Send(buf, sizeof(buf));
#endif // WIN32
}

void NetSocketUV::SendUDP(char *buf)
{
}

//принимаем данные по TCP протоколу
void NetSocketUV::ReciveTCP(void *stream)
{
	size_t len = GetLength();
	char *buf = GetReciveBuffer();
#ifdef WIN32
	if (buf != nullptr)
		Recive((SOCKET)sock, buf, len);
#else
	if (buf != nullptr)
		Recive(buf, len);
#endif // WIN32
	if (udp_tcp)
	{
		uv_buf_t *uv_buf = (uv_buf_t *)buf;
		uv_write_t *wr = new uv_write_t;
		uv_stream_t *client = (uv_stream_t *)stream;
		assert(0 == uv_write(wr, client, uv_buf, 4096, OnWrite));
	}

	free(buf);
}
void NetSocketUV::ReciveUDP()
{
}

bool NetSocketUV::SetConnectedSocketToReadMode(uv_stream_t *stream)
{
	if (udp_tcp)
	{
		std::cout << "Setting connected socket to reading mode!" << std::endl;
		int r = uv_read_start(stream, OnAllocBuffer, OnReadTCP);
		assert(r == 0);
	}
	return false;
}

//callback функция которая вызываемая uv_read_start - записывает полученные данные в массив данных
void NetSocketUV::OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{

	NetSocketUV *socket = new NetSocketUV;

	if (nread < 0)
	{
		if (nread == UV_EOF)
		{
			socket->net->OnLostConnection(socket);
		}
	}
	else
	{
		std::cout << "Reading socket" << std::endl;
		NetBuffer *recv_buff = (NetBuffer *)socket->_net->GetReciveBuffer();
		//assert(buf->base == (char*)recv_buff->GetData());
		recv_buff->SetLength(nread);
		socket->ReciveTCP(stream);
	}
}

//callback функция которая выделяет место под буффер данных
void NetSocketUV::OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	std::cout << "Allocate buffer" << std::endl;
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}

//функция закрывающая libuv сокет
void NetSocketUV::OnCloseSocket(uv_handle_t *handle)
{
	free(handle);
}

void NetSocketUV::OnWrite(uv_write_t *req, int status)
{
	if (status == true)
	{
		fprintf(stderr, "Sending TCP/UDP is fail!\n", uv_err_name(status));
	}
	else
	{
		NetBuffer *nb = new NetBuffer;
		NetSocketUV *ns = (NetSocketUV *)GetPtrSocket(nb->sock);
		char *buf = ns->GetReciveBuffer();
		ns->SendTCP(buf);
		std::cout << "Sending packet" << std::endl;

		free(nb);
	}
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
void NetSocketUV::OnConnect(uv_stream_t *stream, int status)
{
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}

	NetSocketUV *netsock = new NetSocketUV;

	uv_tcp_t *client = new uv_tcp_t;
	memset(client, 0, sizeof(uv_tcp_t *));
	uv_tcp_init(loop, client);
	int r = uv_accept(stream, (uv_stream_t *)client);
	assert(r == 0);
	std::cout << "Net connetction" << std::endl;
	netsock->SetConnectedSocketToReadMode((uv_stream_t *)client);
	if (r)
	{
		// ���������� ���������� �� �������
		uv_close((uv_handle_t *)client, OnCloseSocket); // ������� �����, ������� ��� ������ ��� ����������
	}
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