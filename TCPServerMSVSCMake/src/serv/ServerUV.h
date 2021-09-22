#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "NetSock.h"
#include "NetSocketUV.h"
#include "Net.h"

class Server : public NetSocketUV, public NetSocket, public Net, public NetBuffer {
public:

	Server();
	~Server();
	//�������� ��-�� ��������� ����������� � ������ Net
	NetSocket* NewSocket(Net* net);
	void connect(sockaddr* addr, uv_tcp_t* serv);

};

#endif // !SERVERUV_H
