#pragma once
#include "NetSock.hpp"
#include "utils.hpp"


#ifndef		NETSOCKET_UV
#define		NETSOCKET_UV
struct		NetBufferUV;
#define		SENDER_SIZE_UV sizeof(uv_write_t)

struct NET_SOCKET_PTR
{
	NetSocket* net_socket;
};
struct TCP_SOCKET : public NET_SOCKET_PTR, uv_tcp_t
{
	uv_stream_t* handle;
};
struct UDP_SOCKET : public NET_SOCKET_PTR, uv_udp_t
{
};

struct NetBufferUV : public NET_BUFFER_INDEX
{
	char sender_object[SENDER_SIZE_UV];

	NetBufferUV(int index) : NET_BUFFER_INDEX(index)
	{
	}
	virtual ~NetBufferUV()
	{
		index = 0;
	}

	uv_write_t*					GetPtrWrite()											{ return (uv_write_t*)sender_object; }
	uv_udp_send_t*				GetPtrSend()											{ return (uv_udp_send_t*)sender_object; };
};

class NetSocketUV : public NetSocket
{
public:

								NetSocketUV(Net* net);
	virtual						~NetSocketUV();

	virtual bool				Create(int port, bool udp_tcp, bool listen);
	const char*					GetIP(Net_Address* addr, bool own_or_peer);
	bool						Accept();

	void						ReceiveTCP();
	void						ReceiveUPD();
	void						Destroy();

	static void*				SetupRetranslation(void* argv);
	static void*				WaitingDelay(void* delay);

	static NetSocketUV*			NewSocket(Net* net)										{ return new NetSocketUV(net); }
	uv_loop_t*					getSockLoop()											{ return loop; }
protected:
	friend class				ServerUV;
	void*						sock;
	int							status;
	uv_loop_t*					loop;
};

class ServerUV : public Server
{
public:
	ServerUV();
	virtual						~ServerUV();

	virtual void				Destroy();
	virtual void				StopServer();

	virtual NetSocket*			NewSocket(Net* net);
	virtual NET_BUFFER_INDEX*	NewBuffer(int index);
	virtual void				StartUVServer(bool internet);
	virtual bool				UpdateNet();
protected:
	uv_loop_t					loop;
	virtual Net*				NewNet()														{ return NULL; }
	friend class				NetSocketUV;
};

#endif
void							OnAccept				(uv_stream_t* stream, int status);
void							OnAllocBuffer			(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void							OnReadTCP				(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void							OnReadUDP				(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
void							OnCloseSocket			(uv_handle_t* handle);
void							OnWrite					(uv_write_t* req, int status);
void							StartReadingThread		(void* handle);
uv_tcp_t*						GetPtrTCP				(void* ptr);
uv_udp_t*						GetPtrUDP				(void* ptr);
uv_loop_t*						GetLoop					(Net* net);