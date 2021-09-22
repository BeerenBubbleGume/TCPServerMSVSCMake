#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "NetSock.h"

class NetBuffer : public NetSocket{
public:
	NetBuffer();
	~NetBuffer();

	NetSocket* netsock;
	
	char GetData();
	size_t GetLength();
	void SetLength(ssize_t length);
	NetBuffer* GetReciveBuffer();

};

#endif // NETBUFFER_H

