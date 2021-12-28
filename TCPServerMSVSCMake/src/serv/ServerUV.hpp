#pragma once
#ifndef SERVERUV_H
#include "includes.hpp"

class Server
{
public:
	Server();
	virtual ~Server();
	int connect(bool connection);

protected:

	Net*					net;
	NetSocketUV*			net_sockuv;

	/*CString* GetClientID()												{ return NetSocket::GetClientID(); }
	virtual void SetID(void* NewClient)									{ NetSocket::SetID(NewClient); }*/
	
	/*virtual void SendTCP(NET_BUFFER_INDEX* buf) 						{ net_sockuv->SendTCP(buf); }
	virtual void ReceiveTCP()											{ net_sockuv->ReceiveTCP(); }
	virtual void SendUDP(NET_BUFFER_INDEX* buf) 						{ net_sockuv->SendUDP(buf); }
	virtual void ReceiveUPD()											{ net_sockuv->ReceiveUPD(); }*/

	
};

#endif // !SERVERUV_H
