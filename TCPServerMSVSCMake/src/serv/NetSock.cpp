#include "NetSock.h"

NetSocket::NetSocket()
{
	ClientID = nullptr;
	DataBuff = nullptr;
	SocketCT = 0;
}

NetSocket::~NetSocket()
{
	free(ClientID);
	free(DataBuff);
}

NetSocket* GetPtrSocket(void* ptr)
{
	return *((NetSocket**)ptr);
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket((char*)uv_socket - sizeof(void*));
}

void NetSocket::ReciveTCP();
{

}

void NetSocket::Destroy();
