#pragma once
#ifndef NET_H
#define NET_H
#include <iostream>
#include <cstdlib>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class Net {
public:
	Net();
	~Net();
	int tcp_socket;
	int udp_socket;
	int raw_socket;

	
};

#endif // !NET_H

