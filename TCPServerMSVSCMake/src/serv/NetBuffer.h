#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "Net.h"

class NetBuffer{
public:
	NetBuffer();
	~NetBuffer();
	char* buffer;
	
	Net* GetData();
	void GetLength();
	void SetLength(ssize_t length);
	NetBuffer* GetReciveData();

};

#endif // NETBUFFER_H

