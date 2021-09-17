#pragma once
#ifndef NETBUFFER_H
#define NETBUFFER_H
#include <iostream>
#include <cstdlib>
#include "Net.h"

class NetBuffer {
public:
	NetBuffer();
	~NetBuffer();

	void GetData();
	void GetLength();
	void SetLength(ssize_t length);
};

#endif // NETBUFFER_H

