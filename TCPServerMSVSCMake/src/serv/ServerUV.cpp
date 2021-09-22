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

void Server::connect(sockaddr* addr, uv_tcp_t* serv)
{
	netsockuv->NetSocketUV::GetIP((sockaddr_in*)addr, true);
}

void Server::setup(uv_tcp_t* serv, sockaddr_in* addr)
{

}


