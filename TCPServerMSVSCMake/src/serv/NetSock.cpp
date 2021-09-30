#include "NetSock.hpp"



NetSocket::NetSocket()
{
	addr = NULL;
}

NetSocket::~NetSocket()
{
}

void NetSocket::Destroy()
{
}

void NetSocket::SendMessenge(NET_BUFFER_INDEX* buf, Net_Address* addr)
{
	if (buf)
	{
		// ������ �������

		if (IsTCP())
		{
			SendTCP(buf);
		}
		else
		{
			if (addr)
				*(this->addr) = *addr;
			SendUDP(buf);
		}
	}
}

NetSocket* GetPtrSocket(void* ptr)
{
	return (NetSocket*)ptr;
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket(((char*)uv_socket) - sizeof(void*));
}

NetBuffer::NetBuffer()
{
	owner = NULL;
	max_length = 0;
	length = 0;
	position = 0;
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
	int len = this->length + length;
	if (max_length < len)
	{
		// ��������� ����������
		max_length = len;
		unsigned char* vdata = new unsigned char[max_length];
		memcpy(vdata, this->data, this->length);
		delete[]this->data;
		this->data = vdata;
	}

	if (data)
		memcpy(this->data + this->length, data, length);
	this->length = len;
}

void NetBuffer::Delete(int length)
{
	int size = this->length - length;
	memcpy(data, data + length, size);
	this->length -= size;
}

void Net_Address::FromStringIP(const char* ip)
{
	port = 0;
	address = ip;
	int pos = address.Find(":");
	if (pos != -1)
	{
		// ������� ����
		int len = address.GetLength();
		CString port_str = address.Right(len - pos - 1);
		port = port_str.StringToInt();
		address = address.Left(pos);
	}
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

NET_BUFFER_LIST::NET_BUFFER_LIST() : CArrayBase()
{
	net = NULL;

	k_buffer = 0;
	m_buffer = NULL;

	k_buffer = 10;
	m_buffer = (NET_BUFFER_INDEX**)malloc(k_buffer * sizeof(NET_BUFFER_INDEX*));
	for (int i = 0; i < k_buffer; i++)
		m_buffer[i] = NULL;
	
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
