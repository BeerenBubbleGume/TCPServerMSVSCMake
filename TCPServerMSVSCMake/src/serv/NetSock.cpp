#include "NetSock.hpp"

Net::Net()
{
	net_addr = new sockaddr_in;
	bytes_read = 0;
	ClientID = 0;

#ifdef WIN32
	WSADATA wsdata;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsdata))
	{
		std::cout << "WSAStartup crush!" << std::endl;
		exit(0);
	}
	
	net_addr->sin_family = AF_INET;
	net_addr->sin_port = htons(8000);
	net_addr->sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	net_addr->sin_family = AF_INET;
	net_addr->sin_port = htons(8000);
	net_addr->sin_addr.s_addr = htonl(INADDR_ANY);
#endif // WIN32

}

Net::~Net()
{
	//if(buff_length != 0)
		//buff_length = NULL;
	//if(ClientID)
		//ClientID = NULL;
	//if(net_addr)
		//free(net_addr);
	//if(bytes_read)
		//bytes_read = 0;
}
#ifdef WIN32
bool Net::CreateSocket(void* sockptr, sockaddr_in* addr)
{
	if (sockptr)
	{
		tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (tcp_socket > 0)
		{
			std::cout << "Socet created success!" << std::endl;

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		fprintf(stderr, "Cannot create tcp_socket");
		return false;
	}
	return false;
}

void Net::Connect(sockaddr_in* addr, SOCKET socket)
{
	assert(bind(socket, (sockaddr*)addr, sizeof(addr)) == SOCKET_ERROR);
	assert(listen(socket, 1024) == SOCKET_ERROR);
	assert(accept(socket, (sockaddr*)addr, (int*)sizeof(addr)));
}
char Net::Recive()
{
	return recv(tcp_socket, (char*)recv_buf.DataBuff, recv_buf.length, 0);
}
void Net::Send(char* data, unsigned int len)
{

	while (true)
	{
		bytes_read = send(tcp_socket, data, len, 0);
		if (bytes_read <= 0)
		{
			fprintf(stderr, "sending error!\n");
			break;
		}
	}
}

void Net::closesock(SOCKET sock)
{
	closesocket(sock);
	WSACleanup();
}
#else
bool Net::CreateSocket(void* sockptr, sockaddr_in* addr)
{
	if (sockptr)
	{
		tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (tcp_socket > 0)
		{
			std::cout << "Socet created success!" << std::endl;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		fprintf(stderr, "Cannot create tcp_socket");
		return false;
	}
	return false;
}

void Net::Connect()
{
	socklen_t socklen = ('localhost', 8000);
	if (bind(tcp_socket, (sockaddr*)net_addr, socklen) == 0)
	{
		printf("Bind success\n");
	}
	if (listen(tcp_socket, 1024) == 0)
	{
		printf("Listen success\n");
	}
	int a = accept(tcp_socket, (sockaddr*)net_addr, &socklen);
	if (a)
	{
		fprintf(stderr, "Cannot accept!\n");
	}
}
char Net::Recive()
{
	//tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	//net_addr->sin_family = AF_INET;
	//net_addr->sin_port = htons(8000);
	//net_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	return recv(tcp_socket, DataBuff, buff_length, 0);
}
void Net::Send(char* data, unsigned int len)
{
	while (true)
	{
		if (send(tcp_socket, (char*)data, len, 0) == 0)
			break;
	}
}
void Net::closesock(int socket)
{
	close(socket);
}
#endif // WIN32

NetSocket::NetSocket(Net* net)
{
	addr = nullptr;
	this->net = net;
	receiving_socket = (NetSocket*)malloc(sizeof(NetSocket));
}

NetSocket::~NetSocket()
{
	Destroy();
}

void NetSocket::Destroy()
{
	if(addr)
		free(addr);
	if(receiving_socket)
		free(receiving_socket);
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

void NetSocket::OnLostConnection(void* socket)
{
	Net net;
	if (socket)
	{
		std::cout << "Lost connection with socket!" << std::endl;
#ifdef WIN32
		net.Net::closesock((SOCKET)socket);

#else
		net.Net::closesock(static_cast<int>(reinterpret_cast<intptr_t>(socket)));
#endif // !WIN32

	}
}

bool NetSocket::IsServer()
{
	if (tcp_socket && net_addr)
		return true;
	else
		return false;
}


NetBuffer::NetBuffer()
{
	owner = NULL;
	DataBuff = new unsigned char;
	length = 0;
	position = 0;
}

NetBuffer::~NetBuffer()
{
	/*
	if (DataBuff)
		delete[] DataBuff;
	*/
	if(length != 0)
		length = NULL;
}

size_t NetBuffer::GetLength()
{
	return length;
}

int NetBuffer::HasMessage(NetSocket* sockt)
{
	return 0;
}

void NetBuffer::SetMaxSize(int size)
{
	length = size;
	DataBuff = new unsigned char[length];
}

int NetBuffer::SetLength(unsigned int length)
{
	if (length >= length)
	{
		return length = length;
	}
}

void NetBuffer::Add(int length, void* data)
{
	int len = this->length + length;
	if (length < len)
	{
		// ��������� ����������
		length = len;
		unsigned char* vdata = new unsigned char[length];
		memcpy(vdata, this->DataBuff, this->length);
		delete[] this->DataBuff;
		this->DataBuff = vdata;
	}

	if (data)
		memcpy(this->DataBuff + this->length, data, length);
	this->length = len;
}

void NetBuffer::Delete(int length)
{
	int size = this->length - length;
	memcpy(DataBuff, DataBuff + length, size);
	this->length -= size;
}

Net_Address::Net_Address()
{
	address = NULL;
	port = NULL;
}

Net_Address::~Net_Address()
{
	if (address && port)
	{
		delete[] address;
		port = NULL;
	}
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
	return nullptr;
}

NET_BUFFER_LIST::NET_BUFFER_LIST() : CArrayBase()
{
	net = NULL;
	
	k_buffer = NULL;
	m_buffer = new NET_BUFFER_INDEX*;

	k_buffer = 10;
	m_buffer = (NET_BUFFER_INDEX**)malloc(k_buffer * sizeof(NET_BUFFER_INDEX*));
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
		free(m_buffer);
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
