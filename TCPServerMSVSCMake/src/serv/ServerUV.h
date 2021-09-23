#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "NetSocketUV.h"
#include "Net.h"

extern bool udp_tcp;

class Server : public NetSocketUV {
public:

	Server();
	~Server();
	
	sockaddr_in* net_addr;
	char* IDArray;
	uv_loop_t* loop;
	uv_tcp_t* server;

	NetSocket* NewSocket(Net* net);
	int connect(const char* ip);
	char GetClientID();
	void SetID(void* NewClient);
	
	
};

#endif // !SERVERUV_H
