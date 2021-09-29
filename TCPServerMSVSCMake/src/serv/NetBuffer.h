#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "Net.h"
#include <string>


struct NetBuffer : NET_BUFFER_INDEX
{
public:
	NetBuffer();
	~NetBuffer();
	
	unsigned char* GetData() { return DataBuff; }
	size_t GetLength();
	int SetLength(unsigned int length);

	int ClientID = 0;
	int SocketCT = 0;
	
	void* sock;
	
};

#endif // NETBUFFER_H

