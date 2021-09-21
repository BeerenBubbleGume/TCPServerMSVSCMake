#include "NetSock.h"
//#include <unistd.h>
#include <cstring>

NetSocket::NetSocket()
{
	net = new Net;
	DataBuff = Net::buffer;
	Socket = Net::tcp_socket;
	Net::addr = net_addr;
	SocketCT = 0;
	ClientID = 0;
	IDArray = new char[MAXINT16];
}

NetSocket::~NetSocket()
{

}

NetSocket* GetPtrSocket(void* ptr)
{
	return *((NetSocket**)ptr);
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket((char*)uv_socket - sizeof(void*));
}

char NetSocket::GetClientID()
{
	return ;
}

int NetSocket::SetID(void* NewClient)
{
	int counter = 0;
	if(NewClient != nullptr)
	for (int i = 0; i <= MAXINT16; i++)
	{
		counter = i;
		ClientID = counter;
		*IDArray = counter;
		return ClientID;
	}
	else
	{
		fprintf(stderr, "New client is null!");
		exit(1);
	}
}

void NetSocket::ReciveTCP()
{
	net->Recive();
}

void NetSocket::Destroy()
{
	net->closesock(Socket);
}
