#pragma once
#ifndef NETSOCK_H
#define NETSOCKET_H
#include "ServerUV.h"
#include <sys/types.h>
#include <WinSock2.h>
#include <list>
#include <iostream>
#include "../uv.h"

class NetSocket {

public:
	NetSocket();
	~NetSocket();

	char* ClientID;
	char* DataBuff;
	unsigned SocketCT;

};
struct NET_SOCKET_PRT {
	NetSocket* net_socket;
};
struct TCP_SOCKET : public NET_SOCKET_PRT, uv_tcp_t
{

};
struct UDP_SOCKET : public NET_SOCKET_PRT, uv_udp_t
{

};



#endif // !NETSOCK_H
