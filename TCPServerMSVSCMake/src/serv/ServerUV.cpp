#include "ServerUV.h"

Server::Server()
{
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
		return Create(ip, true, 8000, true);
	}
	else
	{
		fprintf(stderr, "IP is not correct!\n");
		return 1;
	}
}


