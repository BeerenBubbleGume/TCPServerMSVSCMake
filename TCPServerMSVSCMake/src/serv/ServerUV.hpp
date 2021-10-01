#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "NetSocketUV.hpp"

//extern bool udp_tcp;

class Server : public NetSocketUV 
{
public:

	Server(Net* net);
	~Server();
	Net* net;
	char* IDArray;

	int connect(const char* ip);
	std::string GetClientID();
	void SetID(void* NewClient);
	
	
};

#endif // !SERVERUV_H
