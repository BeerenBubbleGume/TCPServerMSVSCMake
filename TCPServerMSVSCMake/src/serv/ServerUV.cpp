#include <stdint.h>
#include "ServerUV.hpp"


void HandlerForRegister(void* clientData)
{
}

Server::Server()
{
	net			 = new Net;
	net_sockuv	 = new NetSocketUV(net);
}

void serverBYEHandler(void* instance, u_int8_t requestBytes)
{
	RTCPInstance* rtcp = (RTCPInstance*)instance;
}

void proxyServerMediaSubsessionAfterPlaying(void* clientData)
{
	Server::RTSPProxyServer* server = (Server::RTSPProxyServer*)clientData;
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(clientData);
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

	uvsocket->Destroy();
}

Server::~Server()
{
	if(net)
		delete net;
	if(net_sockuv)
		delete net_sockuv;
}

int Server::connect(bool connection)
{
	if (connection)
	{
		//udp_tcp = true;
		std::cout << "==========Start server!==========" << std::endl;
		//net_sockuv->GetIP(NULL, true);
		if (net_sockuv->Create(8554, true, true) == true)
		{
			std::cout << "receiving success, start retranslate" << std::endl;
			Server::RTSPProxyServer::StartProxyServer(net_sockuv);
		}
		return 0;
	}
	else
	{
		fprintf(stderr, "Server is not connetcted!\n");
		return 1;
	}
}

Server::RTSPProxyServer* Server::RTSPProxyServer::createNew(UsageEnvironment& env, Port ourPort, UserAuthenticationDatabase* authDatabase, unsigned reclamationSeconds)
{
	int ourSocket4 = setUpOurSocket(env, ourPort, AF_INET);
	if (ourSocket4 == -1) return nullptr;
	int ourSocket6 = setUpOurSocket(env, ourPort, AF_INET6);
	if (ourSocket6 == -1) return nullptr;
	return new Server::RTSPProxyServer(env, ourSocket4, ourSocket6, ourPort, authDatabase, reclamationSeconds);
}


void Server::RTSPProxyServer::anonceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName)
{
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();

	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}

void Server::RTSPProxyServer::StartProxyServer(/*CString* inputURL, */void* Data)
{
	TaskScheduler* newscheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*newscheduler);
	OutPacketBuffer::maxSize = 2000000;
	NetSocketUV* socket = (NetSocketUV*)Data;

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

	ByteStreamFileSource* outSource = ByteStreamFileSource::createNew(*env, "test_h.264");
	const unsigned estimatedSessionBandwidth = 500;
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0';

	RTPSink* outputSink = H264VideoRTPSink::createNew(*env, rtpGS, 96);
	//RTCPInstance* rtcp = RTCPInstance::createNew(*env, rtcpGS, estimatedSessionBandwidth, CNAME, outputSink, outSource, true);

	RTSPProxyServer* server = RTSPProxyServer::createNew(*env, 554);

	const char* streamName = "ServerMedia/"/* + *socket->GetClientID()*/;

	ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName);
	DemandServerMediaSubsession* proxy = Server::DemandServerMediaSubsession::createNew(/*socket->net, */*env, true);

	BasicUDPSink* UDPsink = BasicUDPSink::createNew(*env, rtpGS);
	StreamState* state = new StreamState(*proxy, server->fServerPort, server->fServerPort, outputSink, UDPsink, estimatedSessionBandwidth, outSource, rtpGS, rtcpGS);

	sms->addSubsession(proxy);
	server->addServerMediaSession(sms);

	H264VideoStreamFramer* framer = H264VideoStreamFramer::createNew(*env, outSource, false);
	framer->flushInput();
	outputSink->startPlaying(*outSource, proxyServerMediaSubsessionAfterPlaying, sms);
	RTSPProxyServer::anonceStream(server, sms, streamName);

	server->eventLoopWatchVariable = 0;

	if (!server->isRTSPClient())
		proxy->pauseStream1(server->numClientSessions(), state);

	env->taskScheduler().doEventLoop(&server->eventLoopWatchVariable);

	//return;
}

bool Server::RTSPProxyServer::StopProxyServer(void* clientData)
{
	return false;
}

Server::RTSPProxyServer::~RTSPProxyServer()
{
}

Server::RTSPProxyServer::RTSPProxyServer(UsageEnvironment& env,
	int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
	UserAuthenticationDatabase* authDatabase,
	unsigned reclamationSeconds)
	: RTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort, authDatabase, reclamationSeconds)
{
	eventLoopWatchVariable = 0;
}


Server::DemandServerMediaSubsession* Server::DemandServerMediaSubsession::createNew(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource)
{
	return new DemandServerMediaSubsession(/*net, */env, reuseFirstSource);
}

Server::DemandServerMediaSubsession::~DemandServerMediaSubsession()
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

ServerMediaSession* Server::DemandServerMediaSubsession::createNewSMS(UsageEnvironment& env, const char* fileName, FILE* fid)
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

FramedSource* Server::DemandServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	/*NetBuffer* recvBuff = net->GetRecvBuffer();
	fBuffer = recvBuff->GetData();
	fBufferSize = recvBuff->GetLength();*/

	FILE* in = fopen("test_h.264", "r");

	ByteStreamFileSource* fileSource = ByteStreamFileSource::createNew(envir(), in);
	if (fileSource == NULL) return NULL;
	fBufferSize = fileSource->fileSize();

	// Create a framer for the Video Elementary Stream:
	return H264VideoStreamFramer::createNew(envir(), fileSource);
}

RTPSink* Server::DemandServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
	return SimpleRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 9000, "video", "H264", 1, True, True);
}

void Server::DemandServerMediaSubsession::pauseStream1(unsigned clientID, void* streamToken)
{
	OnDemandServerMediaSubsession::pauseStream(clientID, streamToken);
}

void Server::DemandServerMediaSubsession::subsessionByeHandler(void* clientData)
{
}

void Server::DemandServerMediaSubsession::subsessionByeHandler()
{
}

Server::DemandServerMediaSubsession::DemandServerMediaSubsession(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource) : OnDemandServerMediaSubsession(env, reuseFirstSource)/*, net(net)*/
{
	fBufferSize = 100000000;
	fBuffer = new u_int8_t[fBufferSize];
}
