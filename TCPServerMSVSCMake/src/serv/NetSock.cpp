#include "NetSock.hpp"

int SERVER_DEFAULT_PORT = 8000;

Net::Net()
{
	bytes_read = 0;
	ClientID = 0;
	addr = nullptr;
	receiving_socket = (NetSocket*)malloc(sizeof(NetSocket));
	IDArray = new uint64_t(INT16_MAX);
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
	addr = new Net_Address;
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
	buf.data = nullptr;
	buf.length = len;
	int index = list->AddBuffer(buf);
	NET_BUFFER_INDEX* bi = list->Get(index);
	std::vector<char*> new_buf;
	new_buf.erase(new_buf.begin(), new_buf.end());
	std::copy(bi->GetData().begin(), bi->GetData().end(), std::back_inserter(new_buf));
	Send_Message* sm = reinterpret_cast<Send_Message*>(new_buf.data());
	sm->sender = sender_id;
	sm->len = length;
	
	unsigned char* data_buf = reinterpret_cast<unsigned char*>(new_buf.data());


	if (length)
		memcpy(&(data_buf[struct_size]), data, length);

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

std::string NetSocket::GetClientID()
{
	char ServerPathURL[] = { "rtsp://192.168.0.141:554" };
	char IDPathURL = (char)net->ClientID;
	char PostfixURL[] = { "/mjpeg" };
	std::string URLstr{ ServerPathURL + IDPathURL, PostfixURL };
	return URLstr;
}

void NetSocket::SetID(void* NewClient)
{
	int counter = 0;
	if (NewClient != nullptr)
		for (int i = 0; i <= INT16_MAX; i++)
		{
			counter = i;
			net->ClientID = counter;
			net->IDArray[i] = (unsigned)net->ClientID;
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

	if(size > DataBuff.size())
		DataBuff.resize(size, NULL);
}

void NetBuffer::Clear()
{
	if (!DataBuff.empty())
	{
		DataBuff.erase(DataBuff.begin(), DataBuff.end());
		DataBuff.reserve(0);
	}
	
}

void NetBuffer::SetLength(size_t length)
{
	if(length > DataBuff.size())
		DataBuff.reserve(length);
}

void NetBuffer::Add(int length, void* data)
{
	unsigned char *vdata = new unsigned char[length];
	memcpy(vdata, DataBuff.data(), length);
	DataBuff.erase(DataBuff.begin(), DataBuff.end());
	DataBuff.push_back((char*)data);

	if (data)
		memcpy(&DataBuff + DataBuff.size(), data, length);
	DataBuff.resize(length);
	
}

void NetBuffer::Delete(int length)
{
	int size = this->DataBuff.size() - length;
	//memcpy(&DataBuff, DataBuff + length, size);
	//DataBuff.
	this->DataBuff.resize(-size);
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

//	if (buf->GetData().size() < buffer.length)
//	{
//		buf->SetMaxSize(buffer.length);
//	}
	
	unsigned char* dest = reinterpret_cast<unsigned char*>(buf->GetData().data());
	if (buffer.data)
		memcpy(dest, buffer.data, buffer.length);
	//buf->SetLength(buffer.length);

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
