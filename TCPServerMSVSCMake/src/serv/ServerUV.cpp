#include "ServerUV.h"

Server::Server()
{
	NetBuffer* netbuff = new NetBuffer;
	net_addr = new sockaddr_in;
	//memset(net_addr, 0, sizeof(sockaddr_in*));
	IDArray = new char[1024];

	loop = uv_default_loop();
	server = new uv_tcp_t;

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
			return Create(ip, net_addr, true, 8000, true);
		}
	}
	else
	{
		fprintf(stderr, "IP is not correct!\n");
		return 1;
	}
}

char Server::GetClientID()
{
	return ClientID;
}

void Server::SetID(void* NewClient)
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


