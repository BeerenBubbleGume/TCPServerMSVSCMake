#include "NetBuffer.h"

NetBuffer::NetBuffer()
{
	
}

NetBuffer::~NetBuffer()
{
}

char NetBuffer::GetData()
{
	return (char)DataBuff + buff_length;	
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
