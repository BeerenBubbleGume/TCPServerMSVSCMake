#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "Net.h"
#include <string>


struct NetBuffer : public Net
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

struct NET_BUFFER_INDEX : public NetBuffer
{
public:
	NET_BUFFER_INDEX(int index) : NetBuffer()
	{
		this->index = index;
	}

	int GetIndex() { return index; }
protected:
	int index;
};
struct Net_Address
{

	int port;
	void FromStringIP(const char* ip);
	void Serialize();
};

#endif // NETBUFFER_H

