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
#include "NetSock.h"

class Net : public NetSocket {
public:
	Net();
	~Net();

	
	int listner;
	char* buffer;
	size_t buff_length;
	int bytes_read;
	sockaddr_in addr; //�� ������� sin_family sin_port sin_addr
	
	Net* Recive();
#ifdef WIN32
	void closesock(SOCKET sock);
#else
	void closesock(int sock);
#endif // WIN32

	

};

#endif // !NET_H

