#include "NetSock.h"
//#include <unistd.h>
#include <cstring>
#include <iostream>
#include "Net.h"
#include "NetSocketUV.h"

Net* net;

NetSocket::NetSocket()
{
	net = new Net;
   /*
#ifdef WIN32
	WSADATA wsdata;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsdata))
	{
		std::cout << "WSAStartup crush!" << std::endl;
		exit(0);
	}
#endif // WIN32

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef UNIX
	raw_socket = socket(AF_INET, SOCK_RAW, SO_PROTOCOL);
#else
	raw_socket = socket(AF_INET, SOCK_RAW, 0);
#endif // UNIX

	net_addr.sin_family = AF_INET;
	net_addr.sin_port = htons(8000);
	net_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	*/
	DataBuff = new char[buff_length];
	IDArray = new char[1024];
	bytes_read = 0;
	
}

NetSocket::~NetSocket()
{
}
char NetSocket::GetClientID()
{
	return ClientID;
}

void NetSocket::SetID(void* NewClient)
{
	int counter = 0;
	if (NewClient != nullptr)
		for (int i = 0; i <= MAXINT16; i++)
		{
			counter = i;
			ClientID = counter;
			*IDArray = counter;
		}
	else
	{
		fprintf(stderr, "New client is null!");
		exit(1);
	}
}
/*
void NetSocket::ReciveTCP()
{
	recv((SOCKET)sock, DataBuff, buff_length, 0);
}
*/
void NetSocket::Destroy()
{
	net->closesock(sock);
}

NetSocket* GetPtrSocket(void* ptr)
{
	return *((NetSocket**)ptr);
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket(((char*)uv_socket) - sizeof(void*));
}
