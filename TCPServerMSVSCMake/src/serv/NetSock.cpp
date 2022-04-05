#include "NetSock.hpp"

int SERVER_DEFAULT_PORT = 8000;

Net::Net()
{
	bytes_read = 0;
	udp_tcp = false;
	addr = nullptr;
	receiving_socket = nullptr;
	sending_list.SetOwner(this);
}

Net::~Net()
{
	if (bytes_read != 0)
		bytes_read = 0;
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

bool Net::Create(bool internet)
{
	return false;
}

void Net::Destroy()
{
}

NetSocket::NetSocket(Net* net)
{
	this->net = net;
	addr = nullptr;
	ClientID = 0;
	port = 0;
	udp_tcp = false;
	session_id = 0;
}

NET_BUFFER_INDEX* Net::PrepareMessage(unsigned int sender_id, size_t length, unsigned char* data)
{
	int struct_size = sizeof (Send_Message);
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

unsigned int NetSocket::GetClientID()
{
	return ClientID;
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
	if (DataBuff != NULL)
	{
		delete[] DataBuff;
		DataBuff = nullptr;
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
		buf = new NET_BUFFER_INDEX(index);
		buf->owner = this;
		m_buffer[index] = buf;
	}

	buf->Reset();
	
	int max_len = buf->GetMaxSize();
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

NET_SOCKET_INFO::NET_SOCKET_INFO()
{
	ClientID = 0;
	port = 0;
	sessionID = 0;
	time = 0;
	udp_tcp = false;
}

NET_SOCKET_INFO::~NET_SOCKET_INFO()
{
}

NET_SESSION_INFO::NET_SESSION_INFO(Net* net)
{
	a_client = nullptr;
	c_client = 0;
	fInfo = nullptr;
	fInfoLength = 0;
	net = nullptr;
}

NET_SESSION_INFO::~NET_SESSION_INFO()
{
	Clear();
}

void NET_SESSION_INFO::Clear()
{
}

bool NET_SESSION_INFO::operator==(const NET_SESSION_INFO& si)
{
	return false;
}

NET_SERVER_SESSION::NET_SERVER_SESSION(Net* net) : NET_SESSION_INFO(net)
{
	enabled = true;
	c_client_id = 0;
	a_client_id = nullptr;
	session_index = -1;
}

NET_SERVER_SESSION::~NET_SERVER_SESSION()
{
	if (c_client_id)
	{
		assert(net->IsServer());
		
	}
}

void NET_SERVER_SESSION::AddClient(NetSocket* socket)
{
	unsigned int* va_clientID = new unsigned int[c_client_id + 1];
	for (int i = 0; i < c_client_id; i++)
		va_clientID[i] = a_client_id[i];
	delete[] a_client_id;

	a_client_id[c_client_id] = socket->GetClientID();
	socket->SetSessionID(session_index);
	c_client_id++;
}

SocketList::SocketList() : CArrayBase()
{
	c_socket = 10;
	a_socket = (NetSocket**)malloc(c_socket * sizeof(NetSocket*));
	for (int i = c_socket - 1; i >= 0; i--)
	{
		a_socket[i] = nullptr;
		IncreaseDeleted(i, i);
	}
}

SocketList::~SocketList()
{
	Clear();
}

void SocketList::Clear()
{
	for (int i = 0; i < c_socket; i++)
	{
		if (a_socket[i])
		{
			a_socket[i]->Destroy();
			delete a_socket[i];
			a_socket[i] = nullptr;
		}
	}
	if (a_socket)
	{
		free(a_socket);
		a_socket = nullptr;
	}
	c_socket = 0;
	
}

NetSocket* SocketList::Get(int index)
{
	if (index >= 0 && index < c_socket)
		return a_socket[index];
	return nullptr;
}

int SocketList::AddSocket(NetSocket* client, unsigned int client_id)
{
	int index;
	if (client_id != 0xfffffff)
	{
		index = FromDeletedToExisting(client_id);
	}
	else
		index = FromDeletedToExisting();
	if (index != -1)
	{
		int size = sizeof(NetSocket*);
		int c_client2 = c_socket + c_socket / 4;
		a_socket = (NetSocket**)realloc(a_socket, size * (c_client2));

		for (int i = c_socket; i < c_client2; i++)
			a_socket[i] = nullptr;

		IncreaseDeleted(c_socket, c_client2 - 1);
		index = FromDeletedToExisting();

		c_socket = c_client2;
	}
	a_socket[index] = client;

	return index;
}

bool SocketList::DeleteSocket(int index)
{
	if (index >= 0 && index < c_socket)
	{
		if (a_socket[index])
		{
			a_socket[index]->Destroy();
			delete a_socket[index];
			a_socket[index] = nullptr;
			FromExistingToDeleted(index);

			return true;
		}
	}
	return false;
}

void SocketList::Expand(int max_size)
{
	if (c_socket < max_size)
	{
		int i;
		NetSocket** va_socket = new NetSocket * [max_size];
		for (i = 0; i < c_socket; i++)
			va_socket[i] = a_socket[i];
		for (i = c_socket; i < max_size; i++)
			va_socket[i] = nullptr;

		delete[] a_socket;
		a_socket = va_socket;

		c_socket = max_size;
		IncreaseDeleted(c_socket, max_size - 1);
	}
}

Server::Server()
{
	stop_time = 0;
	stop_server = false;

	max_client = 400;

	c_migration_client = 0;
	a_migration_client = NULL;

	start_time = 0;
}

Server::~Server()
{
}

void Server::ConnectSocket(NetSocket* socket, unsigned int player_id)
{
	int index = sockets.AddSocket(socket, player_id);
	socket->SetClientID(index);
}

bool Server::Create(bool internet)
{
	if (Net::Create(internet))
	{
		NetSocket* socket = NewSocket(this);
		bool is = socket->Create(1885, true, true);
		if (is)
		{
			ConnectSocket(socket);
			assert(socket->GetClientID() == SERVER_ID);

			socket = NewSocket(this);
			if (is)
			{
				ConnectSocket(socket);
				int i;
				a_migration_client = new unsigned int[c_migration_client];
				info = new NET_SESSION_INFO(this);
				unsigned int max_id = 0;
				for (i = 0; i < c_migration_client; i++)
				{
					if (max_id < a_migration_client[i])
						max_id = a_migration_client[i];
				}
				sockets.Expand(max_id + 1);
			}
		}
	}
	return false;
}

NetSocket* Server::GetServerTCPSocket()
{
	int max = sockets.GetMaxCount();
	if (max > 0)
		return sockets.Get(0);
	return NULL;
}

NetSocket* Server::GetServerUDPSocket()
{
	int max = sockets.GetMaxCount();
	if (max > 1)
		return sockets.Get(1);
	return NULL;
}

void Server::Destroy()
{
	sockets.Clear();
	Net::Destroy();
}

void Server::FillServerInfo(NET_SERVER_INFO& info)
{
	info.sessions = &sessions;
	info.sockets = &sockets;

	info.start_time = start_time;

	info.k_accept = count_accept;
}

int Server::AddSessionInfo(NET_SESSION_INFO* session_info, NetSocket* socket)
{
	NET_SERVER_SESSION* session_server = new NET_SERVER_SESSION(this);
	((NET_SESSION_INFO&)*session_server) = *session_info;
	int index = sessions.AddSession(session_server);
	socket->session_id = index;
	session_server->enabled = true;
	session_server->c_client_id = 1;
	session_server->a_client_id = new unsigned int[1];
	session_server->a_client_id[0] = socket->GetClientID();
	return index;
}

NET_SERVER_INFO::NET_SERVER_INFO()
{
	current_time = 0;
	k_accept = 0;
	sessions = nullptr;
	start_time = 0;
	version = 0;
}

void NET_SERVER_INFO::Clear()
{
}

SessionList::SessionList()
{
}

SessionList::~SessionList()
{
}

void SessionList::ReInit()
{
}

void SessionList::Clear()
{
}

NET_SERVER_SESSION* SessionList::Get(int index)
{
	return nullptr;
}

int SessionList::AddSession(NET_SERVER_SESSION* session)
{
	return 0;
}

bool SessionList::DeleteSession(int index)
{
	return false;
}
