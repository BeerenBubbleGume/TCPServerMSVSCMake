#pragma once
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <vector>
#include "utils.hpp"
#ifdef WIN32
#include <pthread/pthread.h>
#else
#include <pthread.h>
#endif // WIN32


class RTSPProxyServer;
class DemandServerMediaSubsession;
struct StreamClientState;

class DemandServerMediaSubsession : public OnDemandServerMediaSubsession
{
public:
	static DemandServerMediaSubsession* createNew(UsageEnvironment& env, Boolean reuseFirstSource);
	virtual ~DemandServerMediaSubsession();
	static ServerMediaSession* createNewSMS(UsageEnvironment& env, const char* fileName, FILE* fid);
	virtual char const* sdpLines(int addressFamily) { return OnDemandServerMediaSubsession::sdpLines(addressFamily); }

protected:
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
	//int verbosityLevel() const { return ((ProxyServerMediaSession*)fParentSession)->fVerbosityLevel; }
private:

	DemandServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource);
	static void subsessionByeHandler(void* clientData);
	void subsessionByeHandler();

	u_int8_t* fBuffer;
	u_int64_t fBufferSize;
	
};

struct StreamClientState
{
	StreamClientState();
	virtual ~StreamClientState();

	Groupsock* rtpGS;
	Groupsock* rtcpGS;
	MediaSubsessionIterator* it;
	MediaSession* session;
	MediaSubsession* subsession;
	TaskToken streamTimerTask;
	double duration;
};

class NewFramedFilter : public FramedFilter
{
public:
	static NewFramedFilter* createNew(UsageEnvironment& env, FramedSource* inputSource);
	virtual ~NewFramedFilter();
	void reassignInputSource(FramedSource* newInputSource) { fInputSource = newInputSource; }
	friend class ReceivingSource;
	friend class DummySink;
	virtual void doGetNextFrame();

protected:
	NewFramedFilter(UsageEnvironment& env, FramedSource* inputSource);
	unsigned fLimitNumBytesToStream;
	unsigned fNumBytesToStream;
	unsigned fBufferSize;
	u_int64_t fCurIndex;
	unsigned fPreferredFrameSize;
	unsigned fPlayTimePerFrame;
	unsigned fLastPlayTime;

	u_int8_t* fBuffer;

};

class RTSPProxyServer : public RTSPServer
{
public: 
	static RTSPProxyServer* createNew(UsageEnvironment& env, Port ourPort = 554,
		UserAuthenticationDatabase* authDatabase = NULL,
		unsigned reclamationSeconds = 65);

	static void anonceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName);

	static bool StartProxyServer(CString* inputURL, void* Data);
	bool StopProxyServer(void* clientData);
	int getSocket4()										{ return fServerSocketIPv4; }
	int getSocket6()										{ return fServerSocketIPv6; }
	static void ip4SocketHandler(void* data, int mask) { 
		RTSPProxyServer* server = (RTSPProxyServer*)data;
		server->incomingConnectionHandlerIPv4();
	}
	static void ip6SocketHandler(void* data, int mask) {
		RTSPProxyServer* server = (RTSPProxyServer*)data;
		server->incomingConnectionHandlerIPv6();
	}

protected:
	virtual ~RTSPProxyServer();
	RTSPProxyServer(UsageEnvironment& env,
		int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
		UserAuthenticationDatabase* authDatabase,
		unsigned reclamationSeconds);

};

void* runEventLoop(void* incomEnv);
void specRRHandlerFunc(void* clienData);
void subsessionByeHandler(void* clienData);
void proxyServerMediaSubsessionAfterPlaying(void* data);