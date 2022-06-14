#include "NetSock.hpp"

int SERVER_DEFAULT_PORT = 8000;
int PORT_SERVER_TCP = 8080;
#define SERVER_UDP_PORT 4452
#define SERVER_TCP_PORT 7789

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
	return true;
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
	IParr = nullptr;

	IParr = (CString**)malloc(sizeof(CString*) * 100);
}

NET_BUFFER_INDEX* Net::PrepareMessage(unsigned int sender_id, MESSAGE_TYPE type, size_t length, unsigned char* data)
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
	Send_Message* sm = (Send_Message*)buf_data;
	sm->len = len;
	sm->sender = sender_id;
	sm->type = type;


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

void NetSocket::SendMessage(NET_BUFFER_INDEX* buf, Net_Address* addr)
{
	if (buf)
	{
		if (IsTCP())
		{
			SendTCP(buf);
		}
	}
}

bool NetSocket::ReceiveMessages()
{
	bool result = false;
	int next = recvbuffer.HasMessage(this);
	if (next != -1)
	{
		result = true;
		unsigned int clientID = this->ClientID;
		Net* net = this->net;
		unsigned char* data = recvbuffer.GetData();
		do
		{
			int pos = recvbuffer.GetPosition();
			recvbuffer.SetPosition(next);
			unsigned char* current_data = data + pos;
			Send_Message* sm = (Send_Message*)current_data;
			net->receiving_socket = this;
			MESSAGE_TYPE type = sm->type;
			int sender = sm->sender;
			int length = sm->len;
			bool is = net->ReceiveMessage(type, sender, length, current_data + sizeof(Send_Message));
			if (is)
			{
				// ���������� ��������� ����� ������ ���������
				//rtick = net->platform->GetTick();
			}
			else
			{
				// ����� ��������� ��-�� ����������� ���������
				return result;
			}
			next = recvbuffer.HasMessage(this);
		} while (next != -1);
		int pos = recvbuffer.GetPosition();
		int len = recvbuffer.GetLength();
		if (pos == len)
		{
			// � ������ ��� ����������, ���� ������� �� ������
			recvbuffer.SetPosition(0);
			recvbuffer.SetLength(0);
		}
	}
	return result;
}

bool NetSocket::GetIP(CString& addr, bool own_or_peer)
{
	addr = "";
	return true;
}

bool NetSocket::assertIP(CString& addr)
{
	CString** va_addr = new CString*[ClientID + 1];
	for (int i = 0; i < ClientID + 1; ++i)
		va_addr[i] = new CString[100];

	for (int i = 0; i < ClientID; ++i)
		va_addr[i] = IParr[i];
	delete[] IParr;

	for (int i = 0; i < sizeof va_addr; ++i)
	{
		for (int j = 0; j < sizeof va_addr; ++j)
		{
			if (va_addr[i][j] == va_addr[i + 1][j + 1] || va_addr[i][j] == va_addr[i - 1][j - 1])
				return true;
			else
				return false;
		}
	}
	return false;
}

NetSocket* GetPtrSocket(void* ptr)
{
	return *((NetSocket**)ptr);
}

NetSocket* GetNetSocketPtr(void* uv_socket)
{
	return GetPtrSocket(((char*)uv_socket) - sizeof(void*));
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
	int struct_size = sizeof(Send_Message);
	int len = buff_length - position;
	
	int s = position + struct_size;
	if (s <= buff_length)
	{
		// �� ������� ����, ������� ���� ���������
		Send_Message* sm = (Send_Message*)&(DataBuff[position]);
		s += sm->len;
		if (s <= buff_length)
		{
			// ������� ������ ���������
			return s;
		}
	}
	return -1;
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
	this->ip = ip;
	int pos = this->ip.Find(":");
	if (pos != -1)
	{
		// ������� ����
		int len = this->ip.GetLength();
		CString port_str = this->ip.Right(len - pos - 1);
		port = port_str.StringToInt();
		this->ip = this->ip.Left(pos);
	}
	else
		port = SERVER_DEFAULT_PORT;
}

void Net_Address::Serialize(CStream& stream)
{
	if (stream.IsStoring())
	{
		stream << ip;
		stream << port;
	}
	else
	{
		stream >> ip;
		stream >> port;
	}
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
	port = -1;
	sessionID = -1;
	time = 0;
	udp_tcp = false;
}

NET_SOCKET_INFO::~NET_SOCKET_INFO()
{
}

void NET_SOCKET_INFO::Serialize(CStream& stream)
{
	if (stream.IsStoring())
	{
		stream << ClientID;
		stream << sessionID;
		stream << port;
		stream << udp_tcp;
		stream << time;
		stream << ip;
	}
	else
	{
		stream >> ClientID;
		stream >> sessionID;
		stream >> port;
		stream >> udp_tcp;
		stream >> time;
		stream >> ip;
	}
}

NET_SESSION_INFO::NET_SESSION_INFO(Net* net)
{
	a_client = nullptr;
	this->net = net;
	c_client = 0;
	fInfo = nullptr;
	fInfoLength = 0;
}

NET_SESSION_INFO::~NET_SESSION_INFO()
{
	Clear();
}

void NET_SESSION_INFO::Clear()
{
	for (int i = 0; i < c_client; i++)
	{
		delete a_client[i];
		a_client[i] = NULL;
	}

	if (a_client)
	{
		//delete []m_player;
		free(a_client);
		a_client = NULL;
	}

	if (fInfo)
	{
		//delete []info;
		free(fInfo);
		fInfo = NULL;
		fInfoLength = 0;
	}
}

void NET_SESSION_INFO::Serialize(CStream& stream)
{
	if (stream.IsStoring())
	{
		stream << ip;
		stream << name;
		stream << c_client;
		for (int i = 0; i < c_client; i++)
			a_client[i]->Serialize(stream);
		stream << fInfoLength;
		if (fInfoLength > 0)
			stream.Write(fInfo, fInfoLength);
	}
	else
	{
		Clear();
		stream >> ip;
		stream >> name;
		stream >> c_client;
		
		stream >> fInfoLength;
		if (fInfoLength > 0)
		{
			//info=new char[length_info];
			fInfo = (char*)malloc(fInfoLength);
			stream.Read(fInfo, fInfoLength);
		}
	}
}

//bool NET_SESSION_INFO::operator==(const NET_SESSION_INFO& si)
//{
//	return false;
//}

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
		Server* net_server = (Server*)net;
		for (int i = 0; i < c_client_id; i++)
		{
			NetSocket* socket = net_server->sockets.Get(a_client_id[i]);
			socket->SetSessionID(-1);
			net_server->sockets.DeleteSocket(a_client_id[i]);
		}

		if (a_client_id)
		{
			delete[]a_client_id;
			a_client_id = NULL;
		}
		c_client_id = 0;
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

void NET_SERVER_SESSION::Serialize(CStream& stream)
{
	if (stream.IsStoring())
	{
		stream << enabled;
		//stream<<time;
		stream << c_client_id;
		for (int i = 0; i < c_client_id; i++)
			stream << a_client_id[i];
		stream << session_index;
	}
	else
	{
		stream >> enabled;
		//stream>>time;
		int vc_client_id = c_client_id;
		stream >> c_client_id;
		if (vc_client_id != c_client_id)
		{
			if (a_client_id)
			{
				delete[]a_client_id;
				a_client_id = NULL;
			}
			if (c_client_id)
				a_client_id = new unsigned int[c_client_id];
		}

		for (int i = 0; i < c_client_id; i++)
			stream >> a_client_id[i];
		stream >> session_index;
	}

	NET_SESSION_INFO::Serialize(stream);
}

SocketList::SocketList() : CArrayBase()
{
	c_socket = 10000;
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

void SocketList::Serialize(CStream& stream, Net* net)
{
	if (stream.IsStoring())
	{
		CArrayBase::Serialize(stream);

		stream << c_socket;
		for (int i = 0; i < c_socket; i++)
		{
			if (a_socket[i])
			{
				bool is = true;
				stream << is;
				a_socket[i]->Serialize(stream);
			}
			else
			{
				bool is = false;
				stream << is;
			}
		}
	}
	else
	{
		Clear();

		CArrayBase::Serialize(stream);

		stream >> c_socket;
		if (c_socket)
		{
			a_socket = (NetSocket**)malloc(c_socket * sizeof(NetSocket*));
			//m_socket=new GNetSocket*[k_socket];
			for (int i = 0; i < c_socket; i++)
			{
				bool is;
				stream >> is;

				if (is)
				{
					a_socket[i] = net->NewSocket(net);
					a_socket[i]->Serialize(stream);
				}
				else
					a_socket[i] = NULL;
			}
		}
	}
}

int SocketList::AddSocket(NetSocket* client, unsigned int client_id)
{
	int index;
	if (client_id != 0xffffffff)
	{
		index = FromDeletedToExisting(client_id);
	}
	else
		index = FromDeletedToExisting();
	if (index == -1)
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

Server::Server() : Net()
{
	stop_time = 0;
	stop_server = false;

	max_client = 40000;

	c_migration_client = 0;
	a_migration_client = NULL;
	info = nullptr;
	count_accept = 0;
	start_time = 0;
}

Server::~Server()
{
	Destroy();

	if (a_migration_client)
	{
		delete[]a_migration_client;
		a_migration_client = NULL;
	}
	c_migration_client = 0;

	if (info)
	{
		delete info;
		info = NULL;
	}
}

void Server::ConnectSocket(NetSocket* socket, unsigned int client_id)
{
	int index = sockets.AddSocket(socket, client_id);
	
	socket->SetClientID(index);
}

bool Server::Create(bool internet)
{
	if (Net::Create(internet))
	{
		NetSocket* socket = NewSocket(this);
		bool is = socket->Create(PORT_SERVER_TCP, true, true);
		if (is)
		{
			ConnectSocket(socket);
			assert(socket->ClientID == SERVER_ID);

			socket = NewSocket(this);
			if (internet)
			{
				is = socket->Create(SERVER_TCP_PORT, true, true);
			}
			else
				is = socket->Create(SERVER_UDP_PORT, false, true);
			if (is)
			{
				ConnectSocket(socket);
				assert(socket->ClientID == SERVER_ID + 1);
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

				return true;
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

void Server::OnLostConnection(NetSocket* socket)
{
	unsigned int index = socket->GetClientID();

	if (socket->session_id != -1)
	{
		NET_SERVER_SESSION* ss = sessions.Get(socket->session_id);
		assert(ss);

		bool is_host = (ss->a_client_id[0] == index);
		if (is_host)
		{
			ss->enabled = false;
		}

		for (int i = 0; i < ss->c_client_id; i++)
		{
			unsigned int player_id = ss->a_client_id[i];
			if (player_id != index)
			{
				NetSocket* socket_receiver = sockets.Get(player_id);
				NET_BUFFER_INDEX* result = PrepareMessage(SERVER_ID, MESSAGE_TYPE_LOST_CONNECTION, 4, (unsigned char*)&index);
				socket_receiver->SendMessage(result);
			}
		}
	}

	if (index || sockets.Get(0) == socket)
		sockets.DeleteSocket(index);
	else
	{
		int index_nohello = sockets_nohello.FindIndex(socket);
		if (index_nohello != -1)
		{
			sockets_nohello.Delete(index_nohello);
			socket->Destroy();
			delete socket;
		}
	}
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

bool Server::ReceiveMessage(MESSAGE_TYPE type, unsigned sender, unsigned length, unsigned char* data)
{
	switch (type)
	{
	case MESSAGE_TYPE_HELLO:
		break;
	case MESSAGE_TYPE_LOST_CONNECTION:
		break;
	case MESSAGE_TYPE_ENUM_SESSION:
	{	
		int c_session_info = sessions.GetIndexCount();
		if (c_session_info)
		{
			CString ip;
			int c_session_info = sessions.GetIndexCount();
			int i = 0, count_session_info = 0, count_session_info_enabled = 0;
			do
			{
				int index = sessions.GetIndex(i);
				NET_SERVER_SESSION* ss = sessions.Get(index);
				if (ss->enabled)
				{
					count_session_info_enabled++;
				}
				count_session_info++;
				i++;
			} while (i < c_session_info);
			MEM_DATA buf;
			NET_BUFFER_INDEX* result = PrepareMessage(SERVER_ID, MESSAGE_TYPE_ENUM_SESSION, buf.length, buf.data);
			if (sender)
			{
				sockets.Get(sender)->SendMessage(result, NULL);
			}
		}
	}
	break;
	case MESSAGE_TYPE_STOP_SERVER:
	//{
	//	unsigned int time = gettimeofday(tv, nullptr);
	//	if (stop_time && stop_time <= time)
	//	{
	//		unsigned int dtime = time - stop_time;
	//		if (dtime >= 1500) // ��������� ���������� ����� ������� �������
	//		{
	//			if (!stop_server)
	//			{
	//				stop_server = true;
	//				StopServer();
	//			}
	//		}
	//	}
	//	else
	//	{
	//		stop_time = time;
	//		stop_server = false;
	//	}
	//}
	break;
	case MESSEGE_TYPE_SESSION_INFO:
	{
		NET_SESSION_INFO* si = new NET_SESSION_INFO(this);
		NetSocket* socket = sockets.Get(sender);
		if (socket->sessionID != -1)
		{
			NET_SERVER_SESSION* session_server = sessions.Get(socket->sessionID);
		}
		else
		{
			if (socket->sessionID == -1)
				int index = AddSessionInfo(si, socket);
			else
			{
				NET_SERVER_SESSION* session_server = sessions.Get(socket->sessionID);
				si->ip = session_server->ip;
				((NET_SESSION_INFO&)*session_server) = *si;
			}
		}
		delete si;
	}
		break;
	default:
		break;
	}
	return true;
}

NET_SERVER_INFO::NET_SERVER_INFO()
{
	current_time = 0;
	k_accept = 0;
	sessions = nullptr;
	sockets = nullptr;
	start_time = 0;
	version = 0;
}

void NET_SERVER_INFO::Clear()
{
	if (!server_version.IsEmpty())
	{
		server_version.Delete(server_version.GetLength());
	}
	current_time = 0;
	k_accept = 0;
	if (sessions)
	{
		free(sessions);
		sessions = nullptr;
	}
	if (sockets)
	{
		free(sockets);
		sockets = nullptr;
	}
	version = 0;
	start_time = 0;
}

void NET_SERVER_INFO::Serialize(CStream& stream, Net* net)
{
	if (stream.IsStoring())
	{
		stream << version;

		stream << server_version;

		stream << start_time;
		stream << current_time;
		stream << k_accept;

		sessions->Serialize(stream, net);
		sockets->Serialize(stream, net);
	}
	else
	{
		Clear();

		stream >> version;

		stream >> server_version;

		stream >> start_time;
		stream >> current_time;
		stream >> k_accept;

		sessions = new SessionList();
		sessions->Serialize(stream, net);

		sockets = new SocketList();
		sockets->Serialize(stream, net);
	}
}

NET_SERVER_INFO::~NET_SERVER_INFO()
{
	Clear();
}

SessionList::SessionList()
{
	k_session = 0;
	m_session = NULL;
	ReInit();
}

SessionList::~SessionList()
{
	Clear();
}

void SessionList::ReInit()
{
	Clear();
	k_session = 10000;
	m_session = (NET_SERVER_SESSION**)malloc(k_session * sizeof(NET_SERVER_SESSION*));
	for (int i = k_session - 1; i >= 0; i--)
	{
		m_session[i] = nullptr;
		IncreaseDeleted(i, i);
	}
}

void SessionList::Serialize(CStream& stream, Net* net)
{
	if (stream.IsStoring())
	{
		CArrayBase::Serialize(stream);

		stream << k_session;
		for (int i = 0; i < k_session; i++)
		{
			if (m_session[i])
			{
				bool is = true;
				stream << is;
				m_session[i]->Serialize(stream);
			}
			else
			{
				bool is = false;
				stream << is;
			}
		}
	}
	else
	{
		Clear();

		CArrayBase::Serialize(stream);

		stream >> k_session;
		if (k_session)
		{
			//m_session=new NET_SESSION_SERVER*[k_session];
			m_session = (NET_SERVER_SESSION**)malloc(k_session * sizeof(NET_SERVER_SESSION*));
			for (int i = 0; i < k_session; i++)
			{
				bool is;
				stream >> is;

				if (is)
				{
					m_session[i] = new NET_SERVER_SESSION(net);
					m_session[i]->Serialize(stream);
				}
				else
					m_session[i] = NULL;
			}
		}
	}
}

void SessionList::Clear()
{
	for (int i = 0; i < k_session; i++)
	{
		if (m_session[i])
		{
			delete m_session[i];
			m_session[i] = nullptr;
		}
	}
	if (m_session)
	{
		free(m_session);
		m_session = nullptr;
	}
	k_session = 0;
	ClearExistingAndDeleted();
}

NET_SERVER_SESSION* SessionList::Get(int index)
{
	if (index >= 0 && index < k_session)
		return m_session[index];
	return nullptr;
}

int SessionList::AddSession(NET_SERVER_SESSION* session)
{
	int index = FromDeletedToExisting();
	if (index == -1)
	{
		int size = sizeof(NET_SERVER_SESSION*);
		int k_session2 = k_session + k_session / 4;
		m_session = (NET_SERVER_SESSION**)realloc(m_session, size * (k_session2));
		for (int i = k_session; i < k_session2; i++)
			m_session[i] = nullptr;
		IncreaseDeleted(k_session, k_session2 - 1);
		index = FromDeletedToExisting();

		k_session = k_session2;
	}

	m_session[index] = session;
	

	return index;
}

bool SessionList::DeleteSession(int index)
{
	if (index >= 0 && index < k_session)
	{
		if (m_session[index])
		{
			delete m_session[index];
			m_session[index] = NULL;
			FromExistingToDeleted(index);

			return true;
		}
	}
	return false;
}
