#pragma once
#ifndef NET_H
#define NET_H
#include <iostream>
#include <cstdlib>
#ifdef WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#endif // UNIX
#include "NetBuffer.h"

class Net : public NetBuffer 
{
public:
	Net();
	~Net();

#ifdef WIN32
	void closesock(void* sock);
#else
	void closesock(void* sock);
#endif // WIN32

	

};

#endif // !NET_H

