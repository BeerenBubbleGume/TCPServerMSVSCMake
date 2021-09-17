#pragma once
#ifndef NET_H
#define NET_H
#include <iostream>
#include <cstdlib>
#ifdef UNIX
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#else
#include <WinSock2.h>
#endif // UNIX



class Net {
public:
	Net();
	~Net();
	int tcp_socket;
	int udp_socket;
	int raw_socket;

	void GetReciveData();
	
};

#endif // !NET_H

