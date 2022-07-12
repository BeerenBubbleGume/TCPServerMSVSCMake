#pragma once
#include "NetSock.hpp"
#include "utils.hpp"
#include "FF_encoder.hpp"

#ifndef		NETSOCKET_UV
#define		NETSOCKET_UV
struct		NetBufferUV;
#define		SENDER_SIZE_UV sizeof(uv_write_t)
#define		RTSP_PARAM_STRING_MAX 200

#if defined(__WIN32__) || defined(_WIN32) || defined(_QNX4)
#define _strncasecmp _strnicmp
#define snprintf _snprintf
#else
#define _strncasecmp strncasecmp
#endif

struct NET_SOCKET_PTR
{
	NetSocket* net_socket;
};
struct TCP_SOCKET : public NET_SOCKET_PTR, uv_tcp_t
{
};

struct UDP_SOCKET : public NET_SOCKET_PTR, public uv_udp_t
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
};

class NetSocketUV : public NetSocket
{
public:

								NetSocketUV(Net* net);
	virtual						~NetSocketUV();

	virtual bool				Create(int port, bool udp_tcp, bool listen, SOCKET_MODE mode);
	virtual bool				GetIP(CString& addr, bool own_or_peer);
	bool						Accept();
	bool						AcceptRTSP();

	void						ReceiveRTSPcommand();
	void						ReceiveTCP();
	void						ReceiveUPD();
	virtual void				SendTCP(NET_BUFFER_INDEX* buf);
	void						Destroy();

	//virtual void				SetupRetranslation(NetSocketUV* socket, unsigned int clientID);
	static void*				WaitingDelay(void* delay);
#ifndef WIN32
	FF_encoder*					sender;
#endif // !WIN32

protected:
	friend class				ServerUV;
	void*						sock;
	int							status;
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

	bool						parseRTSPRequestString(char const* reqStr, unsigned reqStrSize,
															char* resultCmdName,
															unsigned resultCmdNameMaxSize,
															char* resultURLPreSuffix,
															unsigned resultURLPreSuffixMaxSize,
															char* resultURLSuffix,
															unsigned resultURLSuffixMaxSize,
															char* resultCSeq,
															unsigned resultCSeqMaxSize,
															char* resultSessionIdStr,
															unsigned resultSessionIdStrMaxSize,
															unsigned& contentLength, bool urlIsRTSPS);

	void						handleCmd_DESCRIBE(char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr);
	void						handleCmd_DESCRIBE_afterLookup(NET_SERVER_SESSION* sess);
	void						handleCmd_notFound();

	uv_loop_t					loop;
protected:
	unsigned char				fRequestBuffer[20000];
	unsigned char				fResponseBuffer[20000];
	unsigned char*				fLastCRLF;
	unsigned int				fRequestBytesAlreadySeen;
	const char*					fCurrentCSeq;
	virtual Net*				NewNet()														{ return NULL; }
	friend class				NetSocketUV;

	void						setRTSPResponse(const char* responseStr);

};

#endif
void							OnAccept				(uv_stream_t* stream, int status);
void							OnAllocBuffer			(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void							OnReadTCP				(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void							OnCloseSocket			(uv_handle_t* handle);
void							OnWrite					(uv_write_t* req, int status);
void							OnReadUDP				(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
void							StartReadingThread		(void* handle);
uv_tcp_t*						GetPtrTCP				(void* ptr);
uv_udp_t*						GetPtrUDP				(void* ptr);
uv_loop_t*						GetLoop					(Net* net);
void							SetupRetranslation		(void* net, CString fileName);
void							OnAcceptRTSP			(uv_stream_t* stream, int status);
void							OnReadRTSPcommands		(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
