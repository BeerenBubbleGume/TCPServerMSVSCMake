#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "Net.h"


class NetBuffer : public Net
{
public:
	NetBuffer();
	~NetBuffer();
	
	NetBuffer* GetData();
	size_t GetLength();
	void SetLength(unsigned int length);

	int ClientID = 0;
	int SocketCT = 0;
	
	void* sock;
	int bytes_read;
	Net* net;

};

#endif // NETBUFFER_H

