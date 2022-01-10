#include "NetSock.hpp"

int SERVER_DEFAULT_PORT = 8000;

Net::Net()
{
	bytes_read = 0;
	udp_tcp = false;
	ClientID = 0;
	IDArray = nullptr;
	addr = nullptr;
	receiving_socket = nullptr;
	sending_list.SetOwner(this);
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
	//std::vector<unsigned char> data = recv_buf.GetData().data();
	
}

NetSocket::NetSocket(Net* net)
{
	this->net = net;
	addr = nullptr;
	port = 0;
	udp_tcp = false;
	
}

NET_BUFFER_INDEX* Net::PrepareMessage(unsigned int sender_id, size_t length, unsigned char* data)
{
	int struct_size = sizeof Send_Message;
	size_t len = struct_size + length;
	assert(len > 0);

	NET_BUFFER_LIST* list = GetSendList();
	MEM_DATA buf;
	buf.data = data;
	buf.length = len;
	int index = list->AddBuffer(buf);
	NET_BUFFER_INDEX* bi = list->Get(index);
	
	unsigned char* buf_data = bi->GetData();
	
	if (length)
	{
		memcpy(&(buf_data[struct_size]), data, length);
	}
		

	return bi;
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

const char* NetSocket::GetClientID()
{
	const char* URLstr = new const char(net->GetIDPath());
	return URLstr;
}

void NetSocket::SetID(void* NewClient)
{
	int counter = 0;
	int ID = net->GetIDPath();
	uint16_t* Array = new uint16_t[MAXINT16];
	if (NewClient != nullptr)
	{
		for (int i = 1; i < MAXINT16; i++)
		{
			counter = i;
			ID = counter;
			Array[i] = ID;
			net->setIDPath(ID);
			net->setIDArray(Array);

		}

	}
	else
	{
		fprintf(stderr, "New client is does not exist!");
		exit(1);
	}
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
	owner =nullptr;
	DataBuff = nullptr;
	buff_length = 0;
	max_length = 0;
	position = 0;
}

NetBuffer::~NetBuffer()
{
	Clear();
}

int NetBuffer::HasMessage(NetSocket* sockt)
{
	return 0;
}

void NetBuffer::SetMaxSize(size_t size)
{
	Clear();

	if (size > max_length)
	{
		buff_length = size;
		DataBuff = new unsigned char[buff_length];
	}
}

void NetBuffer::Clear()
{
	if (DataBuff)
	{
		delete[] DataBuff;
		//DataBuff = nullptr;
	}
	max_length = 0;
	buff_length = 0;
	
}

void NetBuffer::SetLength(size_t length)
{
	buff_length = length;
}

void NetBuffer::Add(int length, void* data)
{
	int len = buff_length + length;
	if (max_length < len)
	{
		// ��������� ����������
		max_length = len;
		unsigned char* vdata = new unsigned char[max_length];
		memcpy(vdata, DataBuff, buff_length);
		delete[] DataBuff;
		DataBuff = vdata;
	}
	if (data)
		memcpy(DataBuff + buff_length, data, length);
	buff_length = length;
	
}

void NetBuffer::Delete(int length)
{
	int size = buff_length - length;
	memcpy(&DataBuff, DataBuff + length, size);
	//DataBuff.
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
	m_buffer = (NET_BUFFER_INDEX**)malloc(k_buffer * sizeof(NET_BUFFER_INDEX*));
	//memcpy(m_buffer, 0, k_buffer);
	for (int i = 0; i < k_buffer; i++)
		m_buffer[i] = NULL;
	IncreaseDeleted(0, k_buffer - 1);
	
}

NET_BUFFER_LIST::~NET_BUFFER_LIST()
{
	Clear();
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
	
	int max_len=buf->GetMaxSize();
	if (max_len < buffer.length)
	{
		buf->SetMaxSize(buffer.length);
	}
	
	unsigned char* dest = buf->GetData();
	if (buffer.data)
	{
		memcpy(dest, buffer.data, buffer.length);
	}
	buf->SetLength(buffer.length);

	return index;
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

void NET_BUFFER_LIST::Clear()
{
	
	for (int i = 0; i < k_buffer; i++)
	{
		if (m_buffer[i])
		{
			delete m_buffer[i];
			m_buffer[i] = nullptr;
		}
	}
	if (m_buffer)
	{
		free(m_buffer);
		m_buffer = nullptr;
	}
	k_buffer = 0;
}

NET_BUFFER_INDEX::~NET_BUFFER_INDEX()
{
	if(index != 0)
		index = 0;
}
