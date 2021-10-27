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
	int connect(bool connection);

protected:

	Net* net;
	NetSocketUV* net_sockuv;

	std::string GetClientID()					override						{ return NetSocket::GetClientID(); }
	virtual void SetID(void* NewClient)			override						{ NetSocket::SetID(NewClient); }
	virtual void SendTCP(NET_BUFFER_INDEX* buf) override						{ net_sockuv->SendTCP(buf); }
	virtual void ReceiveTCP()					override						{ net_sockuv->ReceiveTCP(); }
	virtual void SendUDP(NET_BUFFER_INDEX* buf) override						{ net_sockuv->SendUDP(buf); }
	virtual void ReceiveUPD()					override						{ net_sockuv->ReceiveUPD(); }

	
};

#endif // !SERVERUV_H
