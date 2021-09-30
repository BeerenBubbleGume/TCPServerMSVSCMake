#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "NetSocketUV.hpp"
#include "Net.hpp"

//extern bool udp_tcp;

class Server : public NetSocketUV 
{
public:

	Server();
	~Server();
	
	sockaddr_in* net_addr;
	char* IDArray;
	
	int connect(const char* ip);
	char* GetClientID();
	void SetID(void* NewClient);
	
	
};

#endif // !SERVERUV_H
