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

class Net {
public:
	Net();
	~Net();
#ifdef WIN32
	SOCKET tcp_socket;
	SOCKET udp_socket;
	SOCKET raw_socket;
	void closesock(SOCKET sock);
	
#else 
	int tcp_socket;
	int udp_socket;
	int raw_socket;
	void closesock(int socket);
#endif // UNIX
	
	int listner;
	char* buffer;
	size_t buff_ltngth;
	int bytes_read;
	sockaddr_in addr; //�� ������� sin_family sin_port sin_addr

	Net* Recive();
	
};

#endif // !NET_H

