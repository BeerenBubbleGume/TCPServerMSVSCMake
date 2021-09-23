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
	int connect(const char* ip);

};

#endif // !SERVERUV_H
