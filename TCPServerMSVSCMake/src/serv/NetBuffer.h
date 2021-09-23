#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>


class NetBuffer
{
public:
	NetBuffer();
	~NetBuffer();
	
	char GetData();
	size_t GetLength();
	void SetLength(unsigned int length);
	NetBuffer* GetReciveBuffer();

	int ClientID = 0;
	char* DataBuff;
	int SocketCT = 0;
	size_t buff_length;
	void* sock;
	int bytes_read;

};

#endif // NETBUFFER_H

