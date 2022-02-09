#pragma once
#ifndef SERVERUV_H
#include "includes.hpp"
#include "NetSocketUV.hpp"

using namespace live555;

class Server
{
public:
	Server();
	virtual ~Server();
	int connect(bool connection);

protected:

	Net*				net;
	NetSocketUV*	net_sockuv;

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
		void pauseStream1(unsigned clientID, void* streamToken);

	protected:
		virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
		virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
		/*void setNetPtr(Net* net) { this->net = net; }*/

	private:

		DemandServerMediaSubsession(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource);
		static void subsessionByeHandler(void* clientData);
		void subsessionByeHandler();

		u_int8_t* fBuffer;
		u_int64_t fBufferSize;
		friend class RTSPProxyServer;
		/*Net* net;*/
	};
	
};

#endif // !SERVERUV_H
