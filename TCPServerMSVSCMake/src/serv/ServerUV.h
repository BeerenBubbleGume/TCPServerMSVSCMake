#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "NetSock.h"
#include "NetSocketUV.h"

class Server {
public:

	Server();
	~Server();

	NetSocketUV* netsockuv;

	//�������� ��-�� ��������� ����������� � ������ Net
	NetSocket* NewSocket(Net* net);
	void connect(sockaddr* addr, uv_tcp_t* serv);
	void setup(uv_tcp_t* serv, sockaddr_in* addr);

protected:
	
	uv_connect_t* connect_data;

};

#endif // !SERVERUV_H
