#pragma once
#ifndef NETSOCK_H
#define NETSOCKET_H
#include "Net.h"
#include "../../libs/includes/uv.h"

class NetSocket {

public:
	NetSocket();
	~NetSocket();

	Net* net;
	char* ClientID;
	char* DataBuff;
	int SocketCT;
	int Socket;

	void ReciveTCP();
	void Destroy();
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

NetSocket* GetPtrSocket(void* ptr);
NetSocket* GetNetSocketPtr(void* uv_socket);


#endif // !NETSOCK_H
