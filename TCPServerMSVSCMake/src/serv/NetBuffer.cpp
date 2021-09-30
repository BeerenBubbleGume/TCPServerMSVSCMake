#include "NetBuffer.h"

NetBuffer::NetBuffer()
{;
}

NetBuffer::~NetBuffer()
{
	free(DataBuff);
	buff_length = NULL;
}

size_t NetBuffer::GetLength()
{
	return buff_length;
}

int NetBuffer::HasMessage(NetSocket* sockt)
{
	return 0;
}

int NetBuffer::SetLength(unsigned int length)
{
	if (length >= buff_length)
	{
		return buff_length = length;
	}
}

void NetBuffer::Add(int length, void* data)
{
}

void NetBuffer::Delete(int length)
{
}

void Net_Address::FromStringIP(const char* ip)
{
}

void Net_Address::Serialize()
{
}

uv_write_t* NetBufferUV::GetPtrWrite()
{
	return (uv_write_t*)sender_object;
}

uv_udp_send_t* NetBufferUV::GetPtrSend()
{
	return nullptr;
}

NET_BUFFER_LIST::NET_BUFFER_LIST()
{
}

NET_BUFFER_LIST::~NET_BUFFER_LIST()
{
}

int NET_BUFFER_LIST::AddBuffer(const MEM_DATA& buffer)
{
	return 0;
}

void NET_BUFFER_LIST::DeleteBuffer(int index)
{
	if (m_buffer && index >= 0 && index < k_buffer)
	{
		if (m_buffer[index])
		{
			FromExistingToDeleted(index);
		}
	}
}
