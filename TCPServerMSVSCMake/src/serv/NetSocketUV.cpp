/* NetSocket дочерний класс NetSocket используещий событийный цикл из библиотеки libuv
* для преобразования абстракций используются методы из NetSocket (GetPtrSocket, GetNetSocketPtr) и методы: GetPtrTCP, GetLoop, GetPtrUDP
* для корректного запуска требуется в классе сервера вызвать метод GetIP, после чего по цепочки вызывутся все включенные методы класса NetSocketUV
*/

#include "NetSocketUV.h"
#include <cassert>
#include <fstream>
#include <cstring>
#include "Net.h"
#include "ServerUV.h"


bool udp_tcp;

NetSocketUV::NetSocketUV()
{
	status = errno;
	net = new NetBuffer;
}

NetSocketUV::NetSocketUV(Net* _Net)
{
}

NetSocketUV::~NetSocketUV()
{
}

bool NetSocketUV::Create(const char* ip, bool udp_tcp, int port, bool listen)
{
	//NetSocketUV::Create(ip, addr, udp_tcp, port, listen);
	
	if (udp_tcp)
	{
		sock = malloc(sizeof(TCP_SOCKET));
		memset(sock, 0, sizeof(TCP_SOCKET));
		((TCP_SOCKET*)sock)->net_socket = this;

		loop = uv_default_loop();
		uv_loop_init(loop);
		std::cout << "Create socket!" << std::endl;
		GetIP(ip, true);
	}
	
	return false;
}

bool NetSocketUV::GetIP(const char* ip, bool own_or_peer)
{
	sockaddr_in* addr = new sockaddr_in;
	if (own_or_peer) 
	{
		std::cout << "Set IP to socket!" << std::endl;
		assert(0 == uv_ip4_addr(ip, 8000, addr));
		Connect(8000, ip, addr);
		return true;
	}
	return false;
}

bool NetSocketUV::Connect(int port, const char* ip, sockaddr_in* addr)
{
	if ((loop && server) != NULL)
	{
		uv_connect_t* req = new uv_connect_t;
		//uv_tcp_connect(req, server, (sockaddr*)addr, OnConnect);
		
		int init = uv_tcp_init(loop, server);
		if (init)
		{
			fprintf(stderr, "Cannot initialize socket!\n", uv_strerror(init));
			exit(1);
		}	
		int gip = uv_tcp_bind(server, (sockaddr*)addr, 0);
		std::cout << "Binding socket!" << std::endl;
		udp_tcp = true;
		Accept();
	}

	return false;
}

bool NetSocketUV::Accept()
{
	if (udp_tcp)
	{
		std::cout << "Start listening socket!" << std::endl;
		int list = uv_listen((uv_stream_t*)server, 1024, OnConnect);
		if (list != 0)
		{
			fprintf(stderr, "Cannot listen socket!\n");
			exit(2);
		}
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
		exit(4);
	}

}

void NetSocketUV::SendUDP(char* buf)
{
}

//принимаем данные по TCP протоколу
void NetSocketUV::ReciveTCP()
{
	unsigned int arr_length = GetLength();
	char *DataArray = new char;
	for (int i = 0; i <= arr_length; ++i)
	{
		DataArray = GetData();
	}
	SendTCP(DataArray);
	
	free(DataArray);
}

void NetSocketUV::ReciveUDP()
{
}

bool NetSocketUV::SetConnectedSocketToReadMode()
{
	if (udp_tcp)
	{
		std::cout << "Setting connected socket to reading mode!" << std::endl;
		//uv_stream_t* stream = GetPtrStream(net);
		assert(uv_read_start((uv_stream_t*)server, OnAllocBuffer, OnReadTCP) == 0);
		/*if (r)
		{
			fprintf(stderr, "Cannot reading!\n");
			exit(3);
		}
		return (r == 0);*/
	}
	return false;
}

//callback функция которая вызываемая uv_read_start - записывает полученные данные в массив данных
void NetSocketUV::OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
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

//callback функция которая выделяет место под буффер данных
void NetSocketUV::OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) 
{
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

//функция закрывающая libuv сокет
void NetSocketUV::OnCloseSocket(uv_handle_t* handle) {
	free(handle);
}

void NetSocketUV::OnWrite(uv_write_t* req, int status)
{
	if (status != 0)
	{
		fprintf(stderr, "Sending TCP/UDP is fail!\n", uv_strerror(status));
	}
}

//callback функция вызываемая из Accept 
void NetSocketUV::OnConnect(uv_stream_t* stream, int status)
{
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}


	NetSocketUV* netsock = new NetSocketUV;
	NetBuffer* net = new NetBuffer;
	uv_loop_t* servloop = net->loop;

	uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(servloop, client);
	if (uv_accept(stream, (uv_stream_t*)client) == 0)
	{
		std::cout << "Socket have set to read mode!" << std::endl;
		netsock->SetConnectedSocketToReadMode();
	}
	else
	{
		// ���������� ���������� �� �������
		uv_close((uv_handle_t*)client, OnCloseSocket); // ������� �����, ������� ��� ������ ��� ����������
	}

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
			((TCP_SOCKET*)sock)->net_socket = nullptr;
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

uv_stream_t* GetPtrStream(void* ptr)
{
	return (uv_stream_t*)(((char*)ptr) + sizeof(void*));
}

NetBuffer* GetPtrBuffer(void* ptr)
{
	return (NetBuffer*)(((char*)ptr) + sizeof(void*));
}

uv_loop_t* GetLoop(void* prt)
{
	return (uv_loop_t*)(((char*)prt) + sizeof(void*));
}