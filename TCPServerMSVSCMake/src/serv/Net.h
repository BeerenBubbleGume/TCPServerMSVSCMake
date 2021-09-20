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
#pragma comment(lib, "ws2_32.lib")
#endif // UNIX

class Net {
public:
	Net();
	~Net();
#ifdef UNIX
	int tcp_socket;
	int udp_socket;
	int raw_socket;
	void closesock(int socket);
#else 
	SOCKET tcp_socket;
	SOCKET udp_socket;
	SOCKET raw_socket;
	void closesock(SOCKET sock);
#endif // UNIX
	
	int listner;
	char* buffer[4096];
	int bytes_read;
	sockaddr_in addr;

	Net* Recive();

	
};

#endif // !NET_H

