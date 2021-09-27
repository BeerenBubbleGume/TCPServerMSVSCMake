#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "NetSocketUV.h"
#include "Net.h"
#include "NetBuffer.h"

extern bool udp_tcp;

class Server : public NetSocketUV {
public:

	Server();
	~Server();
	
	sockaddr_in* net_addr;
	char* IDArray;
	
	NetSocket* NewSocket(Net* net);
	int connect(const char* ip);
	char GetClientID();
	void SetID(void* NewClient);
	
protected:
	void Accept();
	bool IsConnected();
	
};

#endif // !SERVERUV_H
