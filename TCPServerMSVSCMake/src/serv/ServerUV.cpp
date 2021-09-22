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

int Server::connect(sockaddr* addr, const char* ip)
{
	return GetIP(ip, (sockaddr_in*)addr, true);
}


