#include <stdint.h>
#include "ServerUV.hpp"

Server::Server() : NetSocketUV(net)
{
	net			 = new Net;
	net_sockuv	 = new NetSocketUV(net);
}

Server::~Server()
{
	if(net)
		delete net;
	if(net_sockuv)
		delete net_sockuv;
}

int Server::connect(bool connection)
{
	if (connection)
	{
		//udp_tcp = true;
		std::cout << "==========Start server!==========" << std::endl;
			
		net_sockuv->Create(8000, true, true);

		return 0;
	}
	else
	{
		fprintf(stderr, "Server is not connetcted!\n");
		return 1;
	}
}



