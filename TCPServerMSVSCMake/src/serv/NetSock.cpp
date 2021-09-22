#include "NetSock.h"
//#include <unistd.h>
#include <cstring>

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
	printf("netsock recivetcp");
	net->Recive();
}

void NetSocket::Destroy()
{
	printf("netsock destroy");
	net->closesock(Socket);
}
