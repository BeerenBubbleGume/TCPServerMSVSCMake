#include "NetBuffer.h"
Net* net; //выделить место

NetBuffer::NetBuffer()
{
}

NetBuffer::~NetBuffer()
{
}

Net* NetBuffer::GetData()
{
	return Recive();
}

void NetBuffer::GetLength()
{
	buffer[buff_ltngth];
}

void NetBuffer::SetLength(ssize_t length)
{
	if (length)
	{
		net->buff_ltngth = length;
	}
}

NetBuffer* NetBuffer::GetReciveBuffer()
{
	return (NetBuffer*)(buffer + sizeof(buffer[buff_ltngth]));
}
