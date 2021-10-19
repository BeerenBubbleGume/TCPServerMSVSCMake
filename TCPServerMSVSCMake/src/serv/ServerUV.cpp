#include <stdint.h>
#include "ServerUV.hpp"
#include <string.h>

Server::Server(Net* net) : NetSocketUV(net)
{
	this->net = net;
	net_sockuv = new NetSocketUV(this->net);
	IDArray = new char[1024];
}

Server::~Server()
{
	this->net = nullptr;
	delete[] IDArray;

}

int Server::connect(bool connection)
{
	if (connection)
	{
		//udp_tcp = true;
		std::cout << "_______________________________" << std::endl
				<<
					"==========Start server!==========" << std::endl
				<<
					"__________________________________" << std::endl;

		return Create(8000, true, true);
	}
	else
	{
		fprintf(stderr, "Server is not connetcted!\n");
		return 1;
	}
}

std::string Server::GetClientID()
{
	char ServerPath[] = { "rtsp://192.168.0.109:554/" };
	return ServerPath + net->ClientID;
}

void Server::SetID(void* NewClient)
{
	int counter = 0;
	if (NewClient != nullptr)
		for (int i = 0; i <= INT16_MAX; i++)
		{
			counter = i;
			net->ClientID = counter;
			*IDArray = counter;
		}
	else
	{
		fprintf(stderr, "New client is does not exist!");
		exit(1);
	}
}


