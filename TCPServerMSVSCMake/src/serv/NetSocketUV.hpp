#pragma once
#include "NetSock.hpp"
#include "utils.hpp"

#ifndef NETSOCKET_UV
#define NETSOCKET_UV
struct NetBufferUV;
#define SENDER_SIZE_UV sizeof(uv_write_t)

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
struct POLL_SOCKET : public NET_SOCKET_PTR, uv_poll_t
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

	uv_write_t* GetPtrWrite() {return (uv_write_t*)sender_object;}
	uv_udp_send_t* GetPtrSend() {return (uv_udp_send_t*)sender_object;};
};

class NetSocketUV : public NetSocket
{
public:
	
	NetSocketUV(Net* net);
	virtual ~NetSocketUV();

	void* sock;

	virtual bool Create(int port, bool udp_tcp, bool listen);
	
	//bool SetConnectedSocketToReadMode(uv_stream_t* stream);
	const char* GetIP(Net_Address* addr, bool own_or_peer);
	
	bool Accept(uv_handle_t* handle);
	
	void SetID(void* NewClient)													{ NetSocket::SetID(NewClient); }
	//virtual const char* GetClientID()											{ return NetSocket::GetClientID(); }

	void SendTCP(NET_BUFFER_INDEX* buf);
	void SendUDP(NET_BUFFER_INDEX* buf);
	void ReceiveTCP();
	void ReceiveUPD();
	void Destroy();

	int status;
	static NetSocketUV* NewSocket(Net* net)										{ return new NetSocketUV(net); }
	uv_loop_t* loop;

	
};
#endif

#ifndef SERVER_LIVE
#define SERVER_LIVE
class Server
{
public:
	Server();
	virtual ~Server();
	int connect(bool connection);

protected:

	Net* net;
	NetSocketUV* net_sockuv;

	/*CString* GetClientID()												{ return NetSocket::GetClientID(); }
	virtual void SetID(void* NewClient)									{ NetSocket::SetID(NewClient); }*/

	/*virtual void SendTCP(NET_BUFFER_INDEX* buf) 						{ net_sockuv->SendTCP(buf); }
	virtual void ReceiveTCP()											{ net_sockuv->ReceiveTCP(); }
	virtual void SendUDP(NET_BUFFER_INDEX* buf) 						{ net_sockuv->SendUDP(buf); }
	virtual void ReceiveUPD()											{ net_sockuv->ReceiveUPD(); }*/

public:

	static void GenerateRTSPURL(void* Data) {
		Server::RTSPProxyServer::StartProxyServer(Data);
		return;
	}

	class RTSPProxyServer : public RTSPServer
	{
	public:
		static RTSPProxyServer* createNew(UsageEnvironment& env, Port ourPort = 554,
			UserAuthenticationDatabase* authDatabase = NULL,
			unsigned reclamationSeconds = 65);

		static void anonceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName);

		static void StartProxyServer(/*CString* inputURL, */void* Data);
		bool StopProxyServer(void* clientData);
		int getSocket4() { return fServerSocketIPv4; }
		int getSocket6() { return fServerSocketIPv6; }
		static void ip4SocketHandler(void* data, int mask) {
			RTSPProxyServer* server = (RTSPProxyServer*)data;
			server->incomingConnectionHandlerIPv4();
		}
		static void ip6SocketHandler(void* data, int mask) {
			RTSPProxyServer* server = (RTSPProxyServer*)data;
			server->incomingConnectionHandlerIPv6();
		}
		void resetLoopWatchVaraible(volatile char eventLoopWatchVariable) { this->eventLoopWatchVariable = eventLoopWatchVariable; }
	protected:
		virtual ~RTSPProxyServer();
		RTSPProxyServer(UsageEnvironment& env,
			int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
			UserAuthenticationDatabase* authDatabase,
			unsigned reclamationSeconds);
		friend class DemandServerMediaSubsession;
		friend class NetBuffer;
		volatile char eventLoopWatchVariable;
	};

protected:
	class DemandServerMediaSubsession : public OnDemandServerMediaSubsession
	{
	public:
		static DemandServerMediaSubsession* createNew(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource);
		virtual ~DemandServerMediaSubsession();
		static ServerMediaSession* createNewSMS(UsageEnvironment& env, const char* fileName, FILE* fid);
		virtual char const* sdpLines(int addressFamily) { return OnDemandServerMediaSubsession::sdpLines(addressFamily); }
		
		//void pauseStream1(unsigned clientID, void* streamToken);
		//virtual void pause();

		/*virtual void SetupSinkAndSource(RTPSink* sink, BasicUDPSink* udp, FramedSource* source) {
			this->fUDPSink = udp;
			this->fRTPSink = sink;
			this->fMediaSource = source;
		}*/
	protected:
		virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
		virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
	private:

		DemandServerMediaSubsession(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource);
		static void subsessionByeHandler(void* clientData);
		void subsessionByeHandler();

		//FramedSource* fMediaSource;
		//RTPSink* fRTPSink;
		//BasicUDPSink* fUDPSink;
		//Boolean fAreCurrentlyPlaying;
		//Boolean fReuseFirstSource;
		u_int8_t* fBuffer;
		u_int64_t fBufferSize;
		friend class RTSPProxyServer;
		/*Net* net;*/
	};

};

#endif

void OnAccept(uv_stream_t* stream, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
void OnCloseSocket(uv_handle_t* handle);
void OnWrite(uv_write_t* req, int status);
void onCloseFile(uv_fs_t* req);
void onOpenFile(uv_fs_t* req);
void OnListining(void* tcp);
void OnWriteFile(uv_fs_t* req);
uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);
uv_loop_t* GetLoop(Net* net);



