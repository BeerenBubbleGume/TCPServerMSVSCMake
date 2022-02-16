﻿// RTSP.cpp: определяет точку входа для приложения.
//

#include "RTSP.h"



void serverBYEHandler(void* instance, u_int8_t requestBytes)
{
	RTCPInstance* rtcp = (RTCPInstance*)instance;
}

void proxyServerMediaSubsessionAfterPlaying(void* clientData)
{
	RTSPProxyServer* server = (RTSPProxyServer*)clientData;
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	volatile char eventLoopWatchVariable = 1;
	server->resetLoopWatchVaraible(eventLoopWatchVariable);

	server->close(subsession->sink);
	subsession->sink = nullptr;

	MediaSession& sess = subsession->parentSession();
	MediaSubsessionIterator itr(sess);

	while ((subsession = itr.next()) != nullptr)
	{
		if (subsession->sink)
			return;
	}
}

RTSPProxyServer* RTSPProxyServer::createNew(UsageEnvironment& env, Port ourPort, UserAuthenticationDatabase* authDatabase, unsigned reclamationSeconds)
{
	int ourSocket4 = setUpOurSocket(env, ourPort, AF_INET);
	if (ourSocket4 == -1) return nullptr;
	int ourSocket6 = setUpOurSocket(env, ourPort, AF_INET6);
	if (ourSocket6 == -1) return nullptr;
	return new RTSPProxyServer(env, ourSocket4, ourSocket6, ourPort, authDatabase, reclamationSeconds);
}


void RTSPProxyServer::anonceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName)
{
	char* url = rtspServer->rtspURL(sms);
	//UsageEnvironment& env = rtspServer->envir();

	std::cout << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}

void RTSPProxyServer::StartProxyServer(/*CString* inputURL, */void* Data)
{
	TaskScheduler* newscheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*newscheduler);
	OutPacketBuffer::maxSize = 2000000;

	in_addr in_Addr;

	in_Addr.s_addr = AF_INET;
	unsigned short rtpPortNum = 18885;
	unsigned short rtcpPortNum = rtpPortNum + 1;

	Port rtpPort(rtpPortNum);
	Port rtcpPort(rtcpPortNum);
	unsigned const short ttl = 5000;
	Groupsock* rtpGS = new Groupsock(*env, *(sockaddr_storage*)&in_Addr, rtpPort, ttl);
	Groupsock* rtcpGS = new Groupsock(*env, *(sockaddr_storage*)&in_Addr, rtcpPort, ttl);

	rtpGS->multicastSendOnly();
	rtcpGS->multicastSendOnly();

	ByteStreamFileSource* outSource = ByteStreamFileSource::createNew(*env, "out_h.264");
	const unsigned estimatedSessionBandwidth = 500;
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0';

	RTPSink* outputSink = H264VideoRTPSink::createNew(*env, rtpGS, 96);
	RTSPProxyServer* server = RTSPProxyServer::createNew(*env, 8554);

	const char* streamName = "ServerMedia/"/* + *socket->GetClientID()*/;

	ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName);
	DemandServerMediaSubsession* proxy = DemandServerMediaSubsession::createNew(/*socket->net, */*env, true);

	sms->addSubsession(proxy);
	server->addServerMediaSession(sms);

	H264VideoStreamFramer* framer = H264VideoStreamFramer::createNew(*env, outSource, false);
	framer->flushInput();
	outputSink->startPlaying(*outSource, proxyServerMediaSubsessionAfterPlaying, sms);
	RTSPProxyServer::anonceStream(server, sms, streamName);

	server->eventLoopWatchVariable = 0;
	//proxy->SetupSinkAndSource(outputSink, UDPsink, outSource);
	//if (!server->isRTSPClient())
	//	proxy->pauseStream1(server->numClientSessions(), proxy);

	env->taskScheduler().doEventLoop(&server->eventLoopWatchVariable);

	//return;
}

bool RTSPProxyServer::StopProxyServer(void* clientData)
{
	return false;
}

RTSPProxyServer::~RTSPProxyServer()
{
}

RTSPProxyServer::RTSPProxyServer(UsageEnvironment& env,
	int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
	UserAuthenticationDatabase* authDatabase,
	unsigned reclamationSeconds)
	: RTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort, authDatabase, reclamationSeconds)
{
	eventLoopWatchVariable = 0;
}

DemandServerMediaSubsession* DemandServerMediaSubsession::createNew(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource)
{
	return new DemandServerMediaSubsession(/*net, */env, reuseFirstSource);
}

DemandServerMediaSubsession::~DemandServerMediaSubsession()
{
	if (fBuffer)
	{
		delete[] fBuffer;
		//fBuffer = nullptr;
	}
	fBufferSize = 0;
}

#define NEW_SMS(description) do{\
const char* destStr = description\
", streamed by ProxyServer";\
sms = ServerMediaSession::createNew(env, fileName, fileName, destStr);\
}while(0)

ServerMediaSession* DemandServerMediaSubsession::createNewSMS(UsageEnvironment& env, const char* fileName, FILE* fid)
{
	const char* extension = strrchr(fileName, '.');
	if (extension == nullptr) return NULL;

	ServerMediaSession* sms = nullptr;
	Boolean const reuseSource = false;

	if (strcmp(extension, ".264"))
	{
		NEW_SMS("H.264 Video");
		OutPacketBuffer::maxSize = 2000000;
		sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(env, fileName, True));
	}

	return sms;
}

FramedSource* DemandServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	FILE* in = fopen("out_h.264", "r");

	ByteStreamFileSource* fileSource = ByteStreamFileSource::createNew(envir(), in);
	if (fileSource == NULL) return NULL;
	fBufferSize = fileSource->fileSize();

	// Create a framer for the Video Elementary Stream:
	return H264VideoStreamFramer::createNew(envir(), fileSource);
}

RTPSink* DemandServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
	return SimpleRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 9000, "video", "H264", 1, True, True);
}

void DemandServerMediaSubsession::pauseStream1(unsigned clientID, void* streamToken)
{
	if (fReuseFirstSource) return;
	
	DemandServerMediaSubsession* state = (DemandServerMediaSubsession*)streamToken;
	if (state != NULL) state->pause();
}

void DemandServerMediaSubsession::pause()
{
	if (fRTPSink != NULL) fRTPSink->stopPlaying();
	if (fUDPSink != NULL) fUDPSink->stopPlaying();
	if (fMediaSource != NULL) fMediaSource->stopGettingFrames();
	fAreCurrentlyPlaying = False;
}

void DemandServerMediaSubsession::subsessionByeHandler(void* clientData)
{
}

void DemandServerMediaSubsession::subsessionByeHandler()
{
}

DemandServerMediaSubsession::DemandServerMediaSubsession(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource) : OnDemandServerMediaSubsession(env, reuseFirstSource)/*, net(net)*/
{
	fBufferSize = 100000000;
	fBuffer = new u_int8_t[fBufferSize];
	fMediaSource = nullptr;
	fRTPSink = nullptr;
	fUDPSink = nullptr;
	fAreCurrentlyPlaying = false;
	fReuseFirstSource = reuseFirstSource;
}


int main(int arc, char* argv[])
{
	RTSPProxyServer::StartProxyServer(argv);

	return 0;
}