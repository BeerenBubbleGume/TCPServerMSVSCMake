#pragma once
#include "utils.hpp"
#include "includes.hpp"

struct	NetBuffer;
struct	Net_Address;
struct	NET_BUFFER_INDEX;
struct	NET_BUFFER_LIST;
struct	MEM_DATA;
struct	SessionList;
class	SocketList;
class	NetSocket;
class	Net;
class	NET_SESSION_INFO;
class	NET_SERVER_SESSION;
class	Server;

#define SERVER_ID 0

struct NetBuffer
{
public:
	NetBuffer();
	~NetBuffer();

	NET_BUFFER_LIST*	owner;
	auto				GetData()																	{ return DataBuff; }
	size_t				GetLength()																	{ return buff_length; }
	void				SetLength(size_t length);
	void				Add(int length, void* data);
	void				Delete(int length);
	int					HasMessage(NetSocket* sockt);
	void				Reset()																		{ position = 0; buff_length = 0; }
	int					GetPosition()																{ return position; }
	void				SetPosition(int pos)														{ position = pos; }
	void				SetMaxSize(size_t size);
	void				Clear();
	unsigned			GetMaxSize()																{ return max_length; }

protected:

	int					position;
	unsigned char*		DataBuff;
	unsigned			buff_length;
	unsigned			max_length;

};

struct NET_BUFFER_LIST : public CArrayBase
{
	Net*				net;
	int					k_buffer;
	NET_BUFFER_INDEX**	m_buffer;

						NET_BUFFER_LIST();
	virtual				~NET_BUFFER_LIST();

	void				SetOwner(Net* owner)															{ net = owner; }

	int					AddBuffer(const MEM_DATA& buffer);
	void				DeleteBuffer(int index);
	void				Clear();
	NET_BUFFER_INDEX*	Get(int index)																	{ return m_buffer[index]; }
};

class NET_SESSION_INFO
{
public:
	NET_SESSION_INFO(Net* net);
	~NET_SESSION_INFO();

	void Clear();

	bool operator==(const NET_SESSION_INFO& si);
	bool operator!=(const NET_SESSION_INFO& si)
	{
		return !(*this == si);
	}

protected:
	CString				ip;
	CString				name;
	NetSocket**			a_client;
	Net*				net;
	unsigned int		c_client;
	int					fInfoLength;
	char*				fInfo;
};

class NET_SERVER_SESSION : public NET_SESSION_INFO
{
public:
	NET_SERVER_SESSION(Net* net);
	virtual ~NET_SERVER_SESSION();

	virtual void		AddClient(NetSocket* socket);

	bool				isEnabled() { return enabled; }
	int					GetClientCount() { return c_client_id; }
	unsigned int*		GetClientArray() { return a_client_id; }
	int					GetSessionIndex() { return session_index; }
protected:
	friend class		Server;
	bool				enabled;
	int					c_client_id;
	unsigned int*		a_client_id;
	int					session_index;
};

struct SessionList : public CArrayBase
{
	int k_session;
	NET_SERVER_SESSION** m_session;

public:
	SessionList();
	virtual ~SessionList();

	void ReInit();

	void Clear();
	NET_SERVER_SESSION* Get(int index);
	int					AddSession(NET_SERVER_SESSION* session);
	bool				DeleteSession(int index);
};

class Net
{
public:
	Net();
	~Net();


	Net_Address*		addr;
	virtual bool		Create(bool internet);
	virtual bool		UpdateNet() = 0;
	virtual void		Destroy();
	virtual NetSocket*	NewSocket(Net* net) = 0;
	NetBuffer*			GetRecvBuffer()																	{ return &recv_buf; }
	void				OnLostConnection(void* sock);
	bool				IsServer()																		{ return true; }
	NET_BUFFER_LIST*	GetSendList()																	{ return &sending_list; }
	NET_BUFFER_INDEX*	PrepareMessage(unsigned int sender_id, size_t length, unsigned char* data);
	NetSocket*			getReceivingSocket()															{ return receiving_socket; }
	void				setupReceivingSocket(NetSocket& socket)											{ receiving_socket = &socket; }
	auto				GetConnectSockaddr()															{ return fConnectionSockaddr; }
	SessionList&		GetSession()																	{ return sessions; }


protected:
	sockaddr			fConnectionSockaddr;
	int					bytes_read;	
	bool				udp_tcp;

	NetSocket*			receiving_socket;
	NET_BUFFER_LIST		sending_list;
	NetBuffer			recv_buf;
	SessionList			sessions;
};

struct NET_SOCKET_INFO
{
	NET_SOCKET_INFO();
	virtual ~NET_SOCKET_INFO();

	void				SetClientID(unsigned int clientID)		{ this->ClientID = clientID; }
	unsigned int		GetClientID()							{ return ClientID; }

	void				SetTime(unsigned int time)				{ this->time = time; }
	unsigned int		GetTime()								{ return time; }

protected:
	unsigned int		ClientID;
	bool				udp_tcp;
	int					port;
	unsigned int		time;
	int					sessionID;
};

class NetSocket : public NET_SOCKET_INFO
{
public:

	NetSocket(Net* net);

	virtual				~NetSocket();
	void				Destroy();

	virtual bool		Create(int port, bool udp_tcp, bool listen);
	virtual void		ReceiveTCP() = 0;
	//virtual void		SetupRetranslation(NetSocket* socket, unsigned int clientID) = 0;
	int					GetSessionID()														{ return sessionID; }
	bool				IsTCP()																{ return udp_tcp; }
	Net*				getNet()															{ return net; }
	Net_Address*		getAddr()															{ return addr; }
	void				SetSessionID(int sessID)											{ session_id = sessID; }
protected:
	Net_Address*		addr;
	Net*				net;
	int					session_id;
	friend class		Server;
};


struct NET_SERVER_INFO
{
	unsigned int		version;

	CString				server_version;

	unsigned int		start_time;
	unsigned int		current_time;

	int					k_accept;

	SessionList*		sessions;
	SocketList*			sockets;

	NET_SERVER_INFO();
	void				Clear();
	virtual				~NET_SERVER_INFO();
};

class SocketList : public CArrayBase
{
	int					c_socket;
	NetSocket**			a_socket;

public:
	SocketList();
	virtual				~SocketList();
	void				Clear();
	NetSocket*			Get(int index);
	int					GetMaxCount() { return c_socket; }
	int					AddSocket(NetSocket* client, unsigned int client_id = 0xffffffff);
	bool				DeleteSocket(int index);
	void				Expand(int max_size);
};


class Server : public Net
{
public:
	Server();
	virtual				~Server();
	void				ConnectSocket(NetSocket* socket, unsigned int client_id = 0xffffffff);
	virtual bool		Create(bool internet);
	virtual bool		IsServer()																{ return true; }
	
	NetSocket*			GetServerTCPSocket();
	NetSocket*			GetServerUDPSocket();

	virtual void		StopServer() = 0;
	virtual bool		UpdateNet() = 0;
	virtual void		Destroy();
	void				FillServerInfo(NET_SERVER_INFO& info);

	int					AddSessionInfo(NET_SESSION_INFO* session_info, NetSocket* socket);
protected:
	friend class		NetSocket;
	friend class		NET_SERVER_SESSION;
	SocketList			sockets;
	NET_SESSION_INFO*	info;
	CArrayPtr			sockets_nohello;
	unsigned int*		a_migration_client;
	int					c_migration_client;
	int					start_time;
	unsigned int		count_accept;
	int					stop_time;
	bool				stop_server;
	int					max_client;
	
};

struct Send_Message
{
	unsigned int		sender;
	int					type;
	int					len;
};

NetSocket* GetPtrSocket(void* ptr);
NetSocket* GetNetSocketPtr(void* uv_socket);

struct NET_BUFFER_INDEX : public NetBuffer
{
	NET_BUFFER_INDEX(int index) : NetBuffer()
	{
		this->index = index;
	}
	~NET_BUFFER_INDEX();
	int GetIndex() { return index; }
protected:
	int index;
};

struct Net_Address
{
	CString address;
	int port;

	void				FromStringIP(const char* ip);
	void				Serialize(CString* stream);
};

struct MEM_DATA
{
	unsigned char* data;
	int length;

	bool operator==(const MEM_DATA& d)
	{
		if (length == d.length)
		{
			for (int i = 0; i < length; i++)
				if (data[i] != d.data[i])
					return false;
			return true;
		}
		return false;
	}
};

