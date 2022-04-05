#pragma once
#include "utils.hpp"
#include "includes.hpp"

struct	NetBuffer;
struct	Net_Address;
struct	NET_BUFFER_INDEX;
struct	NET_BUFFER_LIST;
struct	MEM_DATA;
class	NetSocket;
class	Net;

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

class Net
{
public:
	Net();
	~Net();


	Net_Address*		addr;

	NetBuffer*			GetRecvBuffer()																	{ return &recv_buf; }
	void				OnLostConnection(void* sock);
	bool				IsServer()																		{ return true; }
	NET_BUFFER_LIST*	GetSendList()																	{ return &sending_list; }
	NET_BUFFER_INDEX*	PrepareMessage(unsigned int sender_id, size_t length, unsigned char* data);
	NetSocket*			getReceivingSocket()															{ return receiving_socket; }
	void				setupReceivingSocket(NetSocket& socket)											{ receiving_socket = &socket; }
	auto				GetConnectSockaddr()															{ return fConnectionSockaddr; }
protected:
	sockaddr			fConnectionSockaddr;
	int					bytes_read;	
	NetBuffer			recv_buf;
	bool				udp_tcp;
	NetSocket*			receiving_socket;
	NET_BUFFER_LIST		sending_list;
};

class NetSocket
{
public:

	NetSocket(Net* net);

	virtual			~NetSocket();
	void			Destroy();

	virtual bool	Create(int port, bool udp_tcp, bool listen);

	virtual void	SendTCP(NET_BUFFER_INDEX* buf) = 0;
	virtual void	SendUDP(NET_BUFFER_INDEX* buf) = 0;

	void			SetID(NetSocket* NewClient);
	unsigned int	GetClientID();
	virtual void	ReceiveTCP() = 0;
	virtual void	ReceiveUPD() = 0;

	bool			IsTCP()																{ return udp_tcp; }
	Net*			getNet()															{ return net; }
	Net_Address*	getAddr()															{ return addr; }
protected:
	bool			udp_tcp;
	int				port;
	Net_Address*	addr;
	Net*			net;
	int				ClientID;
	unsigned int*	IDArray;
};

struct Send_Message
{
	unsigned int	sender;
	int				type;
	int				len;
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

	void FromStringIP(const char* ip);
	void Serialize(CString* stream);
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

