#include <stdint.h>
#include "ServerUV.hpp"
#include <string.h>

Server::Server(Net* net) : NetSocketUV(net)
{
	this->net = net;
	//NetBuffer* netbuff = new NetBuffer;
	IDArray = new char[1024];
}

Server::~Server()
{
	this->net = nullptr;
	delete[] IDArray;

}

int Server::connect(const char* ip)
{
	if (ip)
	{
		//udp_tcp = true;
		std::cout << "_______________________________" << std::endl
				<<
					"==========Start server!==========" << std::endl
				<<
					"__________________________________" << std::endl;
		return Create(ip, true, 8000, true);
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
		fprintf(stderr, "New client is null!");
		exit(1);
	}
}


