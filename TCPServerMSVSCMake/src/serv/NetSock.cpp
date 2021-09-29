#include "NetSock.h"
//#include <unistd.h>
#include <cstring>
#include <iostream>
#include "Net.h"
#include "NetSocketUV.h"


NetSocket::NetSocket()
{
}

NetSocket::~NetSocket()
{
}

void NetSocket::Destroy()
{

}

NetSocket* GetPtrSocket(void* ptr)
{
	return (NetSocket*)ptr;
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket(((char*)uv_socket) - sizeof(void*));
}