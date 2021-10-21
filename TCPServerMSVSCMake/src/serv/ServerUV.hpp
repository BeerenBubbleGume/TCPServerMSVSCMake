#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "NetSocketUV.hpp"

//extern bool udp_tcp;

class Server : NetSocket
{
public:
	Server();
	virtual ~Server();
	Net* net;
	NetSocketUV* net_sockuv;
	char* IDArray;

	int connect(bool connection);

	std::string GetClientID();
	virtual void SetID(void* NewClient) override;
	virtual void SendTCP(NET_BUFFER_INDEX* buf) override	{ net_sockuv->SendTCP(buf); }
	virtual void ReceiveTCP() override						{ net_sockuv->ReceiveTCP(); }
	virtual void SendUDP(NET_BUFFER_INDEX* buf) override	{ net_sockuv->SendUDP(buf); }
	virtual void ReceiveUPD() override						{ net_sockuv->ReceiveUPD(); }

	
};

#endif // !SERVERUV_H
