#include <stdint.h>
#include "ServerUV.h"
#include <string.h>
Server::Server()
{
	//NetBuffer* netbuff = new NetBuffer;
	Net* net = new Net;
	IDArray = new char[1024];
}

Server::~Server()
{
}

NetSocket* Server::NewSocket(Net* net)
{
	return new NetSocketUV(net);
}

int Server::connect(const char* ip)
{
	if (ip)
	{
		udp_tcp = true;
		if (net_addr)
		{
			return Create(ip, true, 8000, true);
		}
	}
	else
	{
		fprintf(stderr, "Server is not connetcted!\n");
		return 1;
	}
}

char* Server::GetClientID()
{
	char ServerPath[] = { "rtsp://192.168.0.109:554/" };
	return ServerPath + ClientID;
}

void Server::SetID(void* NewClient)
{
	int counter = 0;
	if (NewClient != nullptr)
		for (int i = 0; i <= INT16_MAX; i++)
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


