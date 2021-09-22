#include "NetBuffer.h"
Net* net;

NetBuffer::NetBuffer()
{
}

NetBuffer::~NetBuffer()
{
}

Net* NetBuffer::GetData()
{
	return net->Recive();
}

void NetBuffer::GetLength()
{
}

void NetBuffer::SetLength(ssize_t length)
{
}

NetBuffer* NetBuffer::GetReciveData()
{
	return nullptr;
}
