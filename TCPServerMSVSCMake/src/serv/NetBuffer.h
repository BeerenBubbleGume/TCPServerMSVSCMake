#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "Net.h"

class NetBuffer : public Net{
public:
	NetBuffer();
	~NetBuffer();
	char* buffer;
	ssize_t buflength;
	
	Net* GetData();
	void GetLength();
	void SetLength(ssize_t length);
	NetBuffer* GetReciveBuffer();

};

#endif // NETBUFFER_H

