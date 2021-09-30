#pragma once
#ifndef NETSOCK_H
#define NETSOCKET_H
#include "NetBuffer.h"
#include "Net.h"
#include "../../libs/includes/uv.h"
#include "utils.hpp"

bool udp_tcp;

class NetSocket	: public Net
{

public:

	Net_Address* addr;
	Net* net;
	NetSocket* receiving_socket;
	NetSocket();
	~NetSocket();
	NetBuffer rbuffer;
	void Destroy();
	NetBuffer* GetReciveBuffer() { return &rbuffer; }	

	virtual void SendTCP(NET_BUFFER_INDEX* buf) = 0;
	virtual void SendUDP(NET_BUFFER_INDEX* buf) = 0;

	virtual void ReceiveTCP() = 0;
	virtual void ReceiveUPD() = 0;
	void SendMessenge(NET_BUFFER_INDEX* buf, Net_Address* addr);
	bool IsTCP() { return udp_tcp; }
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

struct Send_Message
{
	unsigned int sender;
	int type;
	int len;
};

NetSocket* GetPtrSocket(void* ptr);
NetSocket* GetNetSocketPtr(void* uv_socket);




#endif // !NETSOCK_H
