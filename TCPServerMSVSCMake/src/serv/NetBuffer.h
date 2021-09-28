#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "Net.h"
#include <string>


class NetBuffer : public Net
{
public:
	NetBuffer();
	~NetBuffer();
	
	bool GetData();
	size_t GetLength();
	unsigned int SetLength(unsigned int length);
	static char* GetReciveBuffer();


	Net* net;
	int ClientID = 0;
	int SocketCT = 0;
	
	void* sock;
	int bytes_read;
	char* DataBuff;
	size_t buff_length;
};

#endif // NETBUFFER_H

