#include "NetSock.hpp"

int SERVER_DEFAULT_PORT = 8000;
int PORT_SERVER_TCP = 8080;

Net::Net()
{
	wr1 = new CMemWriter;
	wr2 = new CMemWriter;
	rd1 = new CMemReader;
	rd2 = new CMemReader;


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
	addr = new Net_Address;
	ClientID = 0;
	port = 0;
	udp_tcp = false;
	type_license = -1;
	license = NULL;

	//IParr = new CString[10240];

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

bool NetSocket::Create(int port, bool udp_tcp, bool listen, SOCKET_MODE mode)
{
	this->udp_tcp = udp_tcp;
	this->port = port;
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

bool NetSocket::assertIP(CString* IParr, CString& assertedIP)
{
	CString* va_str = IParr;
	
	assert(va_str[0]);

	for (int i = 0; i < 10240; i++)
	{
		if (va_str[i] == assertedIP)
			return true;
		/*for (int j = 0; j < 30; j++)
		{
			for (int ii = 0; ii < 10240; ii++)
			{
				for (int jj = 0; jj < 30; jj++)
					if (va_str[i][j] == va_str[ii][jj])
						return true;
			}
		}*/
	}
	return false;

	/*CStringArray* va_addr = addr;
	
	for (int i = 0; i < ClientID; i++)
	{
		if (va_addr[i].Get(ClientID) == va_addr[i].Get(ClientID - 1) || va_addr[i].Get(ClientID) == va_addr[i].Get(ClientID + 1))
			return true;
		else
			return false;
	}
	return false;*/
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
	fPacketCount = 0;
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

bool NET_SESSION_INFO::operator==(const NET_SESSION_INFO& si)
{
	return false;
}

char* NET_SERVER_SESSION::generateSDPDescription(int address_famaly)
{
	struct sockaddr_storage ourAddress;
	if (address_famaly == AF_INET) {
		ourAddress.ss_family = AF_INET;
		((sockaddr_in&)ourAddress).sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	else { // IPv6
		ourAddress.ss_family = AF_INET6;
		for (unsigned i = 0; i < 16; ++i) {
			((sockaddr_in6&)ourAddress).sin6_addr.s6_addr[i] = inet_addr("127.0.0.1")[&i];
		}
	}

	CAddressString ipAddressStr(ourAddress);
	unsigned ipAddressStrSize = strlen(ipAddressStr.val());

	char* sourceFilterLine;
	if (fIsSSM) {
		char const* const sourceFilterFmt =
			"a=source-filter: incl IN %s * %s\r\n"
			"a=rtcp-unicast: reflection\r\n";
		unsigned const sourceFilterFmtSize
			= strlen(sourceFilterFmt) + 3/*IP4 or IP6*/ + ipAddressStrSize + 1;

		sourceFilterLine = new char[sourceFilterFmtSize];
		sprintf(sourceFilterLine, sourceFilterFmt,
			address_famaly == AF_INET ? "IP4" : "IP6",
			ipAddressStr.val());
	}
	else {
		sourceFilterLine = strDup("");
	}

	char* rangeLine = NULL;
	char* sdp = NULL;

	do {
		unsigned sdpLength = 0;
		NET_SERVER_SUBSESSION* subsession;
		for (subsession = fSubsessionsHead; subsession != NULL;
			subsession = subsession->fNext) {
			char const* sdpLines = subsession->sdpLines(address_famaly);
			if (sdpLines == NULL) continue; 
			sdpLength += strlen(sdpLines);
		}
		if (sdpLength == 0) break; 

		float dur = duration();
		if (dur == 0.0) {
			rangeLine = strDup("a=range:npt=now-\r\n");
		}
		else if (dur > 0.0) {
			char buf[100];
			sprintf(buf, "a=range:npt=0-%.3f\r\n", dur);
			rangeLine = strDup(buf);
		}
		else { // subsessions have differing durations, so "a=range:" lines go there
			rangeLine = strDup("");
		}

		char const* const sdpPrefixFmt =
			"v=0\r\n"
			"o=- %ld%06ld %d IN %s %s\r\n"
			"s=%s\r\n"
			"i=%s\r\n"
			"t=0 0\r\n"
			"a=tool:%s%s\r\n"
			"a=type:broadcast\r\n"
			"a=control:*\r\n"
			"%s"
			"%s"
			"a=x-qt-text-nam:%s\r\n"
			"a=x-qt-text-inf:%s\r\n"
			"%s";
		sdpLength += strlen(sdpPrefixFmt)
			+ 20 + 6 + 20 + 3/*IP4 or IP6*/ + ipAddressStrSize
			+ strlen(fDescriptionSDPString)
			+ strlen(fInfoSDPString)
			+ strlen("RTSP LIBUV CUSTOM") + strlen("1.0.0.0")
			+ strlen(sourceFilterLine)
			+ strlen(rangeLine)
			+ strlen(fDescriptionSDPString)
			+ strlen(fInfoSDPString)
			+ strlen(fMiscSDPLines);
		sdpLength += 1000; // in case the length of the "subsession->sdpLines()" calls below change
		sdp = new char[sdpLength];
		if (sdp == NULL) break;

		// Generate the SDP prefix (session-level lines):
		snprintf(sdp, sdpLength, sdpPrefixFmt,
			fCreationTime.tv_sec, fCreationTime.tv_usec, // o= <session id>
			1, 
			address_famaly == AF_INET ? "IP4" : "IP6",
			ipAddressStr.val(), // o= <address>
			fDescriptionSDPString, // s= <description>
			fInfoSDPString, // i= <info>
			"RTSP LIBUV CUSTOM", "1.0.0.0", // a=tool:
			sourceFilterLine, // a=source-filter: incl (if a SSM session)
			rangeLine, // a=range: line
			fDescriptionSDPString, // a=x-qt-text-nam: line
			fInfoSDPString, // a=x-qt-text-inf: line
			fMiscSDPLines); // miscellaneous session SDP lines (if any)

		// Then, add the (media-level) lines for each subsession:
		char* mediaSDP = sdp;
		for (subsession = fSubsessionsHead; subsession != NULL;
			subsession = subsession->fNext) {
			unsigned mediaSDPLength = strlen(mediaSDP);
			mediaSDP += mediaSDPLength;
			sdpLength -= mediaSDPLength;
			if (sdpLength <= 1) break; // the SDP has somehow become too long

			char const* sdpLines = subsession->sdpLines(address_famaly);
			if (sdpLines != NULL) snprintf(mediaSDP, sdpLength, "%s", sdpLines);
		}
	} while (0);

	delete[] rangeLine; delete[] sourceFilterLine;
	return sdp;
}

float NET_SERVER_SESSION::duration() const
{
	float minSubsessionDuration = 0.0;
	float maxSubsessionDuration = 0.0;
	for (NET_SERVER_SUBSESSION* subsession = fSubsessionsHead; subsession != NULL;
		subsession = subsession->fNext) {
		// Hack: If any subsession supports seeking by 'absolute' time, then return a negative value, to indicate that only subsessions
		// will have a "a=range:" attribute:
		char* absStartTime = NULL; char* absEndTime = NULL;
		subsession->getAbsoluteTimeRange(absStartTime, absEndTime);
		if (absStartTime != NULL) return -1.0f;

		float ssduration = subsession->duration();
		if (subsession == fSubsessionsHead) { // this is the first subsession
			minSubsessionDuration = maxSubsessionDuration = ssduration;
		}
		else if (ssduration < minSubsessionDuration) {
			minSubsessionDuration = ssduration;
		}
		else if (ssduration > maxSubsessionDuration) {
			maxSubsessionDuration = ssduration;
		}
	}

	if (maxSubsessionDuration != minSubsessionDuration) {
		return -maxSubsessionDuration; // because subsession durations differ
	}
	else {
		return maxSubsessionDuration; // all subsession durations are the same
	}
}

NET_SERVER_SESSION::NET_SERVER_SESSION(Net* net, char const* streamName,
	char const* info,
	char const* description,
	bool isSSM,
	char const* miscSDPLines) : NET_SESSION_INFO(net)
{
	enabled = true;
	c_client_id = 0;
	a_client_id = nullptr;
	session_index = -1;

	fIsSSM = false;

	fStreamName = strDup(streamName == NULL ? "" : streamName);

	char* libNamePlusVersionStr = NULL; // by default
	if (info == NULL || description == NULL) {
		libNamePlusVersionStr = new char[strlen("RTSP LIBUV CUSTOM") + strlen("1.0.0.0") + 1];
		sprintf(libNamePlusVersionStr, "%s%s", "RTSP LIBUV CUSTOM", "1.0.0.0");
	}
	fInfoSDPString = strDup(info == NULL ? libNamePlusVersionStr : info);
	fDescriptionSDPString = strDup(description == NULL ? libNamePlusVersionStr : description);
	delete[] libNamePlusVersionStr;

	fMiscSDPLines = strDup(miscSDPLines == NULL ? "" : miscSDPLines);
#ifndef WIN32
	gettimeofday(&fCreationTime, NULL);
#else
	GetLocalTime((LPSYSTEMTIME&)fCreationTime);
#endif // !WIN32\

	
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
		bool is = socket->Create(SERVER_TCP_PORT, internet, true, SOCKET_MODE_TCP);
		if (is)
		{
			ConnectSocket(socket);
			assert(socket->ClientID == SERVER_ID);

			/*socket = NewSocket(this);
			if (internet)
			{
				is = socket->Create(8080, true, true, SOCKET_MODE_TCP);
			}
			else
				is = socket->Create(SERVER_UDP_PORT, false, true, SOCKET_MODE_DEFAULT);*/

			if (is)
			{
				/*ConnectSocket(socket);
				assert(socket->ClientID == SERVER_ID + 1);*/
				int i;

				socket = NewSocket(this);
				if (socket)
				{
					socket->Create(8554, true, true, SOCKET_MODE_RTSP);
					ConnectSocket(socket);
					assert(socket->ClientID == SERVER_ID + 1);
				}

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

	if (socket->sessionID != -1)
	{
		NET_SERVER_SESSION* ss = sessions.Get(socket->sessionID);
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
				//NetSocket* socket_receiver = sockets.Get(player_id);
				//NET_BUFFER_INDEX* result = PrepareMessage(SERVER_ID, MESSAGE_TYPE_LOST_CONNECTION, 4, (unsigned char*)&index);
				//socket_receiver->SendMessage(result);
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
	socket->sessionID = index;
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

	{
		int index_nohello = sockets_nohello.FindIndex(receiving_socket);
		if (index_nohello != -1)
			sockets_nohello.Delete(index_nohello);

		rd1->Start(data);
		int vlen = length;
		if (vlen > 4)
		{
			int version;
			(*rd1) >> version;
			vlen -= 4;
			if (!version)
			{
				int client_version;
				CString name;
				int license_type;
				CString license;

				bool is = false;
				if (vlen > 4)
				{
					(*rd1) >> client_version;
					vlen -= 4;

					int crypt_rnd = 0;
					unsigned char slen;
					if (vlen > 1)
					{
						(*rd1) >> slen;
						int name_len = slen;
						vlen -= 1;
						if (vlen > name_len)
						{
							name.SetLength(name_len);
							rd1->Read((void*)name.c_str(), name_len);
							vlen -= name_len;
							if (vlen > 4)
							{

								(*rd1) >> license_type;
								vlen -= 4;
								if (vlen > 1)
								{
									(*rd1) >> slen;
									name_len = slen;
									vlen -= 1;
									if (vlen == name_len)
									{
										license.SetLength(name_len);
										rd1->Read((void*)license.c_str(), name_len);


										is = true;
									}
								}
							}
						}
					}
				}

				if (is)
				{
					if (IsMigration())
					{
						if (sender && (!migration_ok))
						{
							bool is = false;
							bool is_host = (a_migration_client[c_migration_client - 1] == sender);
							bool has_session = (sessions.GetIndexCount() > 0);
							if (is_host)
							{
								if (!has_session)
								{
									receiving_socket->name = name;
									ConnectSocket(receiving_socket, sender);
									AddSessionInfo(info, receiving_socket);
									delete info;
									info = NULL;

									IsAllClientsMigrated(&name, &license);
									is = true;
								}
							}
							else
							{
								if (has_session)
								{
									NetSocket* migration_player = sockets.Get(sender);
									if (!migration_player)
									{
										receiving_socket->name = name;
										ConnectSocket(receiving_socket, sender);
										int index = sessions.GetIndex(0);
										NET_SERVER_SESSION* ss = sessions.Get(index);
										ss->AddClient(receiving_socket);

										IsAllClientsMigrated(&name, &license);
										is = true;
									}
								}
							}

							if (!is)
							{
								unsigned int current_time;
#ifdef WIN32
								tm tv_ms;
								current_time = _getsystime(&tv_ms);
#else
								current_time = gettimeofday(&tv, nullptr);
#endif

								
								unsigned int dtime = current_time - start_time;
								if (dtime > 7000)
								{
									for (int i = 0; i < c_migration_client; i++)
									{
										NetSocket* socket = sockets.Get(a_migration_client[i]);
										if (socket)
										{
											for (int j = 0; j < c_migration_client; j++)
											{
												if (!sockets.Get(a_migration_client[j]))
												{
													NET_BUFFER_INDEX* result2 = PrepareMessage(SERVER_ID, MESSAGE_TYPE_LOST_CONNECTION, 4, (unsigned char*)&(a_migration_client[i]));
													socket->SendMessage(result2);
												}
											}
											SendMigration(a_migration_client[i], &name, &license);
										}
									}
									migration_ok = true;
								}
							}
						}
					}
					else
					{
						MEM_DATA buf;
						wr1->Finish(buf);
						if (SendHelloReply(receiving_socket, buf, name, license_type, license))
						{
							return true;
						}
						return false;
					}
				}
			}
		}
	}
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

void Server::SendMigration(unsigned int receiver, CString* name, CString* license)
{
	wr1->Start();

	MEM_DATA buf;
	wr1->Finish(buf);
	NET_BUFFER_INDEX* result = PrepareMessage(SERVER_ID, MESSAGE_TYPE_MIGRATION_OK, buf.length, buf.data);
	NetSocket* socket = sockets.Get(receiver);
	socket->SendMessage(result);
}

bool Server::IsAllClientsMigrated(CString* name, CString* license)
{
	bool is = true;
	for (int i = 0; i < c_migration_client; i++)
	{
		if (!sockets.Get(a_migration_client[i]))
		{
			is = false;
			break;
		}
	}

	if (is)
	{
		for (int i = 0; i < c_migration_client; i++)
		{
			SendMigration(a_migration_client[i], name, license);
		}
		migration_ok = true;
	}
	return is;
}

bool Server::SendHelloReply(NetSocket* socket, MEM_DATA& buf, const char* name, int type_license, const char* license)
{
	bool is_ok = false;
	if (buf.length)
	{
		NET_BUFFER_INDEX* result = PrepareMessage(SERVER_ID, MESSAGE_TYPE_HELLO_REPLY, buf.length, buf.data);
		socket->SendMessage(result);
		if (socket->ClientID)
		{
			CString lic = license;
			if (statistics[type_license].clients.Add(lic))
			{
				MEM_DATA data_player_id;
				data_player_id.length = sizeof(socket->ClientID);
				data_player_id.data = (unsigned char*)&(socket->ClientID);
				CString sname = name;
				names.Add(sname, &data_player_id);

				socket->type_license = type_license;
				socket->license = statistics[type_license].clients.Find(lic);
				is_ok = true;
			}
		}
	}

	if (!is_ok)
	{
		socket->Destroy();
		delete socket;
		socket = NULL;

		if (!udp_tcp)
		{
			if (sockets.GetIndexCount() == 2)
			{
				StopServer();
			}
		}
	}
	return is_ok;
}

NET_SERVER_INFO::NET_SERVER_INFO()
{
	current_time = 0;
	k_accept = 0;
	
	start_time = 0;
	version = 0;

	sockets = new SocketList;
	sessions = new SessionList;
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

SessionList::SessionList() : CArrayBase()
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

char* NET_SERVER_SUBSESSION::rtpmapLine()
{
	char* encodingParamsPart;
	encodingParamsPart = strDup("");

	char const* const rtpmapFmt = "a=rtpmap:%d %s/%d%s\r\n";
	unsigned rtpmapLineSize = strlen(rtpmapFmt)
		+ 3 /* max char len */ + strlen("video H264")
		+ 20 /* max int len */ + strlen(encodingParamsPart);
	char* rtpmapLine = new char[rtpmapLineSize];
	sprintf(rtpmapLine, rtpmapFmt,
		96, "video H264",
		0, encodingParamsPart);
	delete[] encodingParamsPart;
	return rtpmapLine;
}

char* NET_SERVER_SUBSESSION::rangeSDPLine()
{
	char* absStart = NULL; char* absEnd = NULL;
	getAbsoluteTimeRange(absStart, absEnd);
	if (absStart != NULL) {
		char buf[100];

		if (absEnd != NULL) {
			sprintf(buf, "a=range:clock=%s-%s\r\n", absStart, absEnd);
		}
		else {
			sprintf(buf, "a=range:clock=%s-\r\n", absStart);
		}
		return strDup(buf);
	}

	if (fParentSession == NULL) return NULL;

	// If all of our parent's subsessions have the same duration
	// (as indicated by "fParentSession->duration() >= 0"), there's no "a=range:" line:
	if (fParentSession->duration() >= 0.0) return strDup("");

	// Use our own duration for a "a=range:" line:
	float ourDuration = duration();
	if (ourDuration == 0.0) {
		return strDup("a=range:npt=now-\r\n");
	}
	else {
		char buf[100];
		sprintf(buf, "a=range:npt=0-%.3f\r\n", ourDuration);
		return strDup(buf);
	}
}

const char* NET_SERVER_SUBSESSION::sdpLines(int addressFamaly)
{
	if (fSDPLines == NULL) {
		in_addr servAddr;
		CAddressString groupAddressStr(servAddr);
		unsigned short portNum = ntohs(8554);
		unsigned char ttl = 255;
		unsigned char rtpPayloadType = 96;
		char const* mediaType = "video H264";
		unsigned estBitrate = 5000;
		char* rtpmapLine = this->rtpmapLine();
		//char* keyMgmtLine = fRTPSink.keyMgmtLine();
		//char const* rtcpmuxLine = rtcpIsMuxed() ? "a=rtcp-mux\r\n" : "";
		char const* rangeLine = rangeSDPLine();
		char const* auxSDPLine = nullptr;
		if (auxSDPLine == NULL) auxSDPLine = "";

		char const* const sdpFmt =
			"m=%s %d RTP/%sAVP %d\r\n"
			"c=IN %s %s/%d\r\n"
			"b=AS:%u\r\n"
			"%s"
			"%s"
			"%s"
			"%s"
			"%s"
			"a=control:%s\r\n";
		unsigned sdpFmtSize = strlen(sdpFmt)
			+ strlen(mediaType) + 5 /* max short len */ + 1 + 3 /* max char len */
			+ 3/*IP4 or IP6*/ + strlen(groupAddressStr.val()) + 3 /* max char len */
			+ 20 /* max int len */
			//+ strlen(rtpmapLine)
			//+ strlen(keyMgmtLine)
			//+ strlen(rtcpmuxLine)
			+ strlen(rangeLine)
			+ strlen(auxSDPLine);
		char* sdpLines = new char[sdpFmtSize];
		sprintf(sdpLines, sdpFmt,
			mediaType, // m= <media>
			portNum, // m= <port>
			//fParentSession->streamingUsesSRTP ? "S" : "",
			rtpPayloadType, // m= <fmt list>
			"IP4", // c= address type
			groupAddressStr.val(), // c= <connection address>
			ttl, // c= TTL
			estBitrate, // b=AS:<bandwidth>
			rtpmapLine, // a=rtpmap:... (if present)
			//keyMgmtLine, // a=key-mgmt:... (if present)
			//rtcpmuxLine, // a=rtcp-mux:... (if present)
			rangeLine, // a=range:... (if present)
			auxSDPLine); // optional extra SDP line
		delete[](char*)rangeLine; /*delete[] keyMgmtLine*/ delete[] rtpmapLine;

		fSDPLines = strDup(sdpLines);
		delete[] sdpLines;
	}

	return fSDPLines;
}
