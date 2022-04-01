// RTSP.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include "utils.hpp"
#ifdef WIN32
#include <WinSock2.h>
#endif // WIN32


class RTSPProxyServer : public RTSPServer
{
public:
	static RTSPProxyServer* 
					createNew(UsageEnvironment& env, Port ourPort = 554,
		UserAuthenticationDatabase* authDatabase = NULL,
		unsigned reclamationSeconds = 65);
	static	void	WhatchAndWait(void* server);
	static	void 	anonceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName);
	static	void	StartProxyServer(char* Data);
	bool			StopProxyServer(void* clientData);
	int				getSocket4()													{ return fServerSocketIPv4; }
	int				getSocket6()													{ return fServerSocketIPv6; }
	static	void 	ip4SocketHandler(void* data, int mask) {
		RTSPProxyServer* server = (RTSPProxyServer*)data;
		server->incomingConnectionHandlerIPv4();
	}
	void	play(void* sock_ptr);
	static void ip6SocketHandler(void* data, int mask) {
		RTSPProxyServer* server = (RTSPProxyServer*)data;
		server->incomingConnectionHandlerIPv6();
	}
	void		resetLoopWatchVaraible(volatile char eventLoopWatchVariable) { this->eventLoopWatchVariable = eventLoopWatchVariable; }
protected:
	virtual		~RTSPProxyServer();
				RTSPProxyServer(UsageEnvironment& env,
		int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
		UserAuthenticationDatabase* authDatabase,
		unsigned reclamationSeconds);

	friend class DemandServerMediaSubsession;
	volatile char eventLoopWatchVariable;
};

class DemandServerMediaSubsession : public OnDemandServerMediaSubsession
{
public:
	static DemandServerMediaSubsession* createNew(UsageEnvironment& env, Boolean reuseFirstSource);
	virtual ~DemandServerMediaSubsession();
	static ServerMediaSession* createNewSMS(UsageEnvironment& env, const char* fileName, FILE* fid);
	virtual char const* sdpLines(int addressFamily) { return OnDemandServerMediaSubsession::sdpLines(addressFamily); }

	void pauseStream1(unsigned clientID, void* streamToken);
	virtual void pause();

	virtual void SetupSinkAndSource(RTPSink* sink, BasicUDPSink* udp, FramedSource* source) {
		this->fUDPSink = udp;
		this->fRTPSink = sink;
		this->fMediaSource = source;
	}
protected:
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
private:

	DemandServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource);
	static void subsessionByeHandler(void* clientData);
	void subsessionByeHandler();

	FramedSource* fMediaSource;
	RTPSink* fRTPSink;
	BasicUDPSink* fUDPSink;
	Boolean fAreCurrentlyPlaying;
	Boolean fReuseFirstSource;
	u_int8_t* fBuffer;
	u_int64_t fBufferSize;
	friend class RTSPProxyServer;
};

void proxyServerMediaSubsessionAfterPlaying(void* clientData);