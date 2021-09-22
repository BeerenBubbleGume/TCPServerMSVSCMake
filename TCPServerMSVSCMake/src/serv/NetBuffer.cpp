#include "NetBuffer.h"

NetBuffer::NetBuffer()
{
	
}

NetBuffer::~NetBuffer()
{
}

NetSocket* NetBuffer::GetData()
{
	return (NetSocket*)netsock->ReciveTCP();
}

size_t NetBuffer::GetLength()
{
	return DataBuff[buff_length];
}

void NetBuffer::SetLength(ssize_t length)
{
	if (length)
	{
		buff_length = length;
	}
}

NetBuffer* NetBuffer::GetReciveBuffer()
{
	return (NetBuffer*)(DataBuff + sizeof(DataBuff[buff_length]));
}
