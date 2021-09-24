#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../uv.h"

class NetBuffer
{
public:
	NetBuffer();
	~NetBuffer();
	
	char GetData();
	size_t GetLength();
	void SetLength(unsigned int length);
	NetBuffer* GetReciveBuffer();
	uv_loop_t* loop;
	uv_tcp_t* server;

	int ClientID = 0;
	char* DataBuff;
	int SocketCT = 0;
	size_t buff_length;
	void* sock;
	int bytes_read;

};

#endif // NETBUFFER_H

