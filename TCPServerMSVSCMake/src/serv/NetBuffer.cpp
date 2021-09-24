#include "NetBuffer.h"

NetBuffer::NetBuffer()
{
	loop = uv_default_loop();
	server = new uv_tcp_t;
	buff_length = 0;
	DataBuff = new char[buff_length];
	bytes_read = 0;
}

NetBuffer::~NetBuffer()
{
}

char NetBuffer::GetData()
{
	return *(char*)(DataBuff + buff_length);	
}

size_t NetBuffer::GetLength()
{
	return DataBuff[buff_length];
}

void NetBuffer::SetLength(unsigned int length)
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

