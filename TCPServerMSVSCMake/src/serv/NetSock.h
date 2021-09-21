#pragma once
#ifndef NETSOCK_H
#define NETSOCKET_H
#include "Net.h"
#include "../../libs/includes/uv.h"
#include "NetBuffer.h"

class NetSocket : public NetBuffer, public Net {

public:
	NetSocket();
	~NetSocket();

	Net* net; //не выделена память \/
	int ClientID; //требуется инициализация 
	char* DataBuff; //требуется выдиление памяти
	int SocketCT;
	int Socket; //не существует, требутеся инициализация
	sockaddr_in net_addr;

	char GetClientID();

	int SetID(void* NewClient);
	char* IDArray;

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
