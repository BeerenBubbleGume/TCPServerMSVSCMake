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
#include <cstdint>
#endif // UNIX

class Net
{
public:
	Net();
	~Net();

	sockaddr_in *net_addr;

#ifdef WIN32
	SOCKET tcp_socket;
	void Connect(sockaddr_in *addr, SOCKET socket);
	char Recive(SOCKET socket, void *buf, size_t len);
	void Send(char *data, size_t len);
	void closesock(SOCKET sock);
	bool CreateSocket(void* sockptr, sockaddr_in* addr);

#else
	int tcp_socket;
	void Connetct(sockaddr_in *addr, int socket);
	char Recive(void *buf, unsigned int len);
	void Send(char *data, unsigned int len);
	void closesock(int sock);
	bool CreateSocket(void *sockptr, sockaddr_in* addr);

#endif // WIN32

	void OnLostConnection(void *socket);
};

#endif // !NET_H
