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

#endif // UNIX

class Net {
public:
	Net();
	~Net();
	struct sockaddr_in* tcp_socket;
	struct sockaddr* udp_socket;
	struct sockaddr* raw_socket;

	
};

#endif // !NET_H

