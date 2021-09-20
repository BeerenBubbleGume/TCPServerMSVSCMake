#include "NetSock.h"

NetSocket::NetSocket()
{
	memset(ClientID, 0, sizeof(char*));
	memset(DataBuff, 0, sizeof(char*));
	SocketCT = 0;
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

void NetSocket::ReciveTCP()
{
	//net->Recive();
}

void NetSocket::Destroy()
{
	//net->closesock(Socket);
}
