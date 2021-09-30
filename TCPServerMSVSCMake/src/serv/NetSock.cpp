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

void NetSocket::SendMessenge(NET_BUFFER_INDEX* buf, Net_Address* addr)
{
	if (buf)
	{
		// ������ �������

		if (IsTCP())
		{
			SendTCP(buf);
		}
		else
		{
			if (addr)
				*(this->addr) = *addr;
			SendUDP(buf);
		}
	}
}

NetSocket* GetPtrSocket(void* ptr)
{
	return (NetSocket*)ptr;
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket(((char*)uv_socket) - sizeof(void*));
}