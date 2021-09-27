#include "NetBuffer.h"

NetBuffer::NetBuffer()
{
	
	bytes_read = 0;
}

NetBuffer::~NetBuffer()
{
}

NetBuffer* NetBuffer::GetData()
{
	return (NetBuffer*)(DataBuff);
}

size_t NetBuffer::GetLength()
{
	return DataBuff[buff_length];
}

void NetBuffer::SetLength(unsigned int length)
{
	if (length > buff_length)
	{
		buff_length = length;
	}
}
