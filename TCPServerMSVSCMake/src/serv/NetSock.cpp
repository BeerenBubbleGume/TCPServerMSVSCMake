#include "NetSock.hpp"

int SERVER_DEFAULT_PORT = 8000;

Net::Net()
{
	bytes_read = 0;
	ClientID = 0;
	addr = nullptr;
	receiving_socket = (NetSocket*)malloc(sizeof(NetSocket));
}

Net::~Net()
{
	if (bytes_read != 0)
		bytes_read = 0;
	if (ClientID != 0)
		ClientID = 0;
	if (addr)
	{
		delete addr;
		addr = nullptr;
	}
	if (receiving_socket)
	{
		delete receiving_socket;
		receiving_socket = nullptr;
	}
}

void Net::ReciveMessege()
{
	int length = recv_buf.GetMaxLength();
	unsigned char* data = recv_buf.GetData();
	
}

NetSocket::NetSocket(Net* net)
{
	this->net = net;
	addr = new Net_Address;
	port = 0;
	udp_tcp = false;

}

NetSocket::~NetSocket()
{
	Destroy();
}

void NetSocket::Destroy()
{
	if (addr)
	{
		delete addr;
		addr = 0;
	}
	if (port != 0)
		port = 0;
	/* if (net)
	* {
	*	delete[] net;
	*	net = nullptr;
	* }
	*/
	if (udp_tcp == true)
		udp_tcp = false;
}

bool NetSocket::Create(int port, bool udp_tcp, bool listen)
{
	this->udp_tcp = udp_tcp;
	this->port = port;
	if (!udp_tcp)
		this->addr = new Net_Address;

	return true;
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
	return *((NetSocket**)ptr);
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket(((char*)uv_socket) - sizeof(void*));
}

void Net::OnLostConnection(void* socket)
{
	
}

NetBuffer::NetBuffer()
{
	owner = nullptr;
	DataBuff = nullptr;
	max_length = 0;
	buff_length = 0;
	position = 0;
}

NetBuffer::~NetBuffer()
{
	Clear();
}

size_t NetBuffer::GetLength()
{
	return buff_length;
}

int NetBuffer::HasMessage(NetSocket* sockt)
{
	return 0;
}

void NetBuffer::SetMaxSize(int size)
{
	Clear();

	buff_length = size;
	DataBuff = new unsigned char[buff_length];
}

void NetBuffer::Clear()
{
	if (DataBuff)
	{
		delete []DataBuff;
		DataBuff = NULL;
	}
	buff_length = 0;
	max_length = 0;
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
	int len = this->buff_length + length;
	if (length < len)
	{
		length = len;
		unsigned char* vdata = new unsigned char[length];
		memcpy(vdata, this->DataBuff, this->buff_length);
		delete[] this->DataBuff;
		this->DataBuff = vdata;
	}

	if (data)
		memcpy(&this->DataBuff, data, length);
	this->buff_length = len;
}

void NetBuffer::Delete(int length)
{
	int size = this->buff_length - length;
	memcpy(DataBuff, DataBuff + length, size);
	this->buff_length -= size;
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
	else
		port = SERVER_DEFAULT_PORT;
}

void Net_Address::Serialize(CString* stream)
{
}

NetBufferUV::~NetBufferUV()
{
	index = NULL;
}

uv_write_t* NetBufferUV::GetPtrWrite()
{
	return (uv_write_t*)sender_object;
}

uv_udp_send_t* NetBufferUV::GetPtrSend()
{
	return (uv_udp_send_t*)sender_object;
}

NET_BUFFER_LIST::NET_BUFFER_LIST() : CArrayBase()
{
	net = nullptr;
	k_buffer = NULL;
	m_buffer = NULL;

	k_buffer = 10;
	m_buffer = new NET_BUFFER_INDEX*[k_buffer];
	memcpy(m_buffer, 0, sizeof(NET_BUFFER_INDEX**));
	for (int i = 0; i < k_buffer; i++)
		m_buffer[i] = NULL;
	IncreaseDeleted(0, k_buffer - 1);
	
}

NET_BUFFER_LIST::~NET_BUFFER_LIST()
{
	for (int i = 0; i < k_buffer; i++)
	{
		if (m_buffer[i])
		{
			delete m_buffer[i];
			m_buffer[i] = NULL;
		}
	}
	
	if (m_buffer)
	{
		delete[] m_buffer;
		m_buffer = NULL;
	}
	
	k_buffer = 0;
}

int NET_BUFFER_LIST::AddBuffer(const MEM_DATA& buffer)
{	
	int index = FromDeletedToExisting();
	if (index == -1)
	{
		int size = sizeof(NET_BUFFER_INDEX*);
		int k_buffer2 = k_buffer + k_buffer / 4;
		m_buffer = (NET_BUFFER_INDEX**)realloc(m_buffer, size * (k_buffer2));

		for (int i = k_buffer; i < k_buffer2; i++)
			m_buffer[i] = NULL;
		IncreaseDeleted(k_buffer, k_buffer2 - 1);
		index = FromDeletedToExisting();
		k_buffer = k_buffer2;
	}

	NET_BUFFER_INDEX* buf = m_buffer[index];
	if (!buf)
	{
		buf = (NET_BUFFER_INDEX*)malloc(sizeof(NET_BUFFER_INDEX));
		buf->owner = this;
		m_buffer[index] = buf;
	}

	buf->Reset();
	int max_len = buf->GetMaxLength();
	if (max_len < buffer.length)
	{
		// ����� ��������� ������ ������
		buf->SetMaxSize(buffer.length);
	}

	unsigned char* dest = buf->GetData();
	if (buffer.data)
		memcpy(dest, buffer.data, buffer.length);
	buf->SetLength(buffer.length);

	return index;
	
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

NET_BUFFER_INDEX::~NET_BUFFER_INDEX()
{
	if(index != 0)
		index = 0;
}
