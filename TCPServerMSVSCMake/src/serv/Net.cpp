#include "Net.h"
#include <cassert>

Net::Net()
{
	net_addr = new sockaddr_in;
	
#ifdef WIN32
	WSADATA wsdata;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsdata))
	{
		std::cout << "WSAStartup crush!" << std::endl;
		exit(0);
	}
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	net_addr->sin_family = AF_INET;
	net_addr->sin_port = htons(8000);
	net_addr->sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	net_addr->sin_family = AF_INET;
	net_addr->sin_port = htons(8000);
	net_addr->sin_addr.s_addr = htonl(INADDR_ANY);
#endif // WIN32

}

Net::~Net()
{
}
#ifdef WIN32
void Net::Connect(sockaddr_in* addr, SOCKET socket)
{
	assert(bind(socket, (sockaddr*)addr, sizeof(addr)) == SOCKET_ERROR);
	assert(listen(socket, 1024) == SOCKET_ERROR);
	assert(accept(socket, (sockaddr*)addr, (int*)sizeof(addr)));
}
char Net::Recive(SOCKET socket, void* buf, size_t len)
{
	return recv(socket, (char*)buf, len, 0);
}
void Net::Send(char* data, size_t len)
{
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	net_addr->sin_family = AF_INET;
	net_addr->sin_port = htons(8000);
	net_addr->sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	while (true)
	{
		if (send(tcp_socket, data, len, 0) == SOCKET_ERROR)
			break;
	}
}

void Net::closesock(SOCKET sock)
{
	closesocket(sock);
	WSACleanup();
}
#else
bool Net::CreateSocket(void* sockptr, sockaddr_in* addr)
{
	if(sockptr)
	{
		int nsock = static_cast<int>(reinterpret_cast<intptr_t>(sockptr));
		nsock = socket(AF_INET, SOCK_STREAM, 0);
		if(nsock > 0)
		{
			printf("Socket created success!\n");
			Connetct(addr, nsock);
			return true;
		}
	}
	else
	{
		fprintf(stderr, "Cannot create tcp_socket");
		return false;
	}
	return false;
}

void Net::Connetct(sockaddr_in* addr, int sock)
{
	socklen_t socklen = ('localhost', 8000);
	if(bind(sock, (sockaddr*)addr, socklen) != 0)
	{
		fprintf(stderr, "Bind error!\n");
		exit(1);
	}
	if(listen(sock, 1024) != 0)
	{
		fprintf(stderr, "Listen error\n");
		exit(2);
	}
	int a = accept(sock, (sockaddr*)addr, (socklen_t*)sizeof(addr));
	if (a)
	{
		fprintf(stderr, "Cannot accept!\n");
	}
}
char Net::Recive(void* buf, unsigned int len)
{
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	net_addr->sin_family = AF_INET;
	net_addr->sin_port = htons(8000);
	net_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	return recv(tcp_socket, (char*)buf, len, 0);
}
void Net::Send(char* data, unsigned int len)
{
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	net_addr->sin_family = AF_INET;
	net_addr->sin_port = htons(8000);
	net_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	while (true)
	{
		if (send(tcp_socket, (char*)data, len, 0) == 0)
			break;
	}
} 
void Net::closesock(int socket)
{
	close(socket);
}
#endif // WIN32


void Net::OnLostConnection(void* socket)
{
	if (socket)
	{
		std::cout << "Lost connection with socket!" << std::endl;
#ifdef WIN32
		closesock((SOCKET)socket);
		
#else
		//closesock((int)socket);
#endif // !WIN32

	}
}

