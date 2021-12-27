#include "ProxyServer.hpp"

DemandServerMediaSubsession* DemandServerMediaSubsession::createNew(UsageEnvironment& env, Boolean reuseFirstSource)
{
	return new DemandServerMediaSubsession(env, reuseFirstSource);
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
	ByteStreamMemoryBufferSource* fileSource = ByteStreamMemoryBufferSource::createNew(envir(), fBuffer, fBufferSize);
	if (fileSource == NULL) return NULL;
	fBufferSize = fileSource->bufferSize();

	// Create a framer for the Video Elementary Stream:
	return H264VideoStreamFramer::createNew(envir(), fileSource);
}

RTPSink* DemandServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
	return SimpleRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 9000, "video", "H264", 1, True, True);
}

DemandServerMediaSubsession::DemandServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource) : OnDemandServerMediaSubsession(env, reuseFirstSource)
{
	fBufferSize = 100000000;
	fBuffer = new u_int8_t[fBufferSize];
}

void DemandServerMediaSubsession::subsessionByeHandler(void* clientData)
{
}

void DemandServerMediaSubsession::subsessionByeHandler()
{
}

StreamClientState::StreamClientState() : it(nullptr), session(nullptr), subsession(nullptr), streamTimerTask(0), duration(0), rtcpGS(nullptr), rtpGS(nullptr)
{
}

StreamClientState::~StreamClientState()
{
	delete it;
	if (session != NULL) {
		// We also need to delete "session", and unschedule "streamTimerTask" (if set)
		UsageEnvironment& env = session->envir(); // alias

		env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
		Medium::close(session);
	}
}

NewFramedFilter* NewFramedFilter::createNew(UsageEnvironment& env, FramedSource* inputSource)
{
	return new NewFramedFilter(env, inputSource);
}

NewFramedFilter::~NewFramedFilter()
{
	delete[] fBuffer;
}

void NewFramedFilter::doGetNextFrame()
{
	if (fCurIndex >= fBufferSize || (fLimitNumBytesToStream && fNumBytesToStream == 0)) {
		handleClosure();
		return;
	}

	// Try to read as many bytes as will fit in the buffer provided (or "fPreferredFrameSize" if less)
	fFrameSize = fMaxSize;
	if (fLimitNumBytesToStream && fNumBytesToStream < (u_int64_t)fFrameSize) {
		fFrameSize = (unsigned)fNumBytesToStream;
	}
	if (fPreferredFrameSize > 0 && fPreferredFrameSize < fFrameSize) {
		fFrameSize = fPreferredFrameSize;
	}

	if (fCurIndex + fFrameSize > fBufferSize) {
		fFrameSize = (unsigned)(fBufferSize - fCurIndex);
	}

	memmove(fTo, &fBuffer[fCurIndex], fFrameSize);
	fCurIndex += fFrameSize;
	fNumBytesToStream -= fFrameSize;

	// Set the 'presentation time':
	if (fPlayTimePerFrame > 0 && fPreferredFrameSize > 0) {
		if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
			// This is the first frame, so use the current time:
			gettimeofday(&fPresentationTime, NULL);
		}
		else {
			// Increment by the play time of the previous data:
			unsigned uSeconds = fPresentationTime.tv_usec + fLastPlayTime;
			fPresentationTime.tv_sec += uSeconds / 1000000;
			fPresentationTime.tv_usec = uSeconds % 1000000;
		}

		// Remember the play time of this data:
		fLastPlayTime = (fPlayTimePerFrame * fFrameSize) / fPreferredFrameSize;
		fDurationInMicroseconds = fLastPlayTime;
	}
	else {
		// We don't know a specific play time duration for this data,
		// so just record the current time as being the 'presentation time':
		gettimeofday(&fPresentationTime, NULL);
	}

	// Inform the downstream object that it has data:
	FramedSource::afterGetting(this);
}

NewFramedFilter::NewFramedFilter(UsageEnvironment& env, FramedSource* inputSource) : FramedFilter(env, inputSource)
{
	fBufferSize = 128 * 1024 * 1024;
	fBuffer = new u_int8_t[fBufferSize];
	fCurIndex = 0;
	fLastPlayTime = 0;
	fLimitNumBytesToStream = 1000000;
	fNumBytesToStream = 0;
	fNumTruncatedBytes = 0;
	fPlayTimePerFrame = 0;
	fPreferredFrameSize = 0;
}

void proxyHandlerForRegister(RTSPServer* rtspServer, unsigned requestId, int resultCode, char* resultString)
{
}

void HandlerForRegister(RTSPServer* rtspServer, unsigned requestId, int resultCode, char* resultString)
{
}

RTSPProxyServer* RTSPProxyServer::createNew(UsageEnvironment& env, Port ourPort, UserAuthenticationDatabase* authDatabase, unsigned reclamationSeconds)
{
	int ourSocket = setUpOurSocket(env, ourPort, AF_INET);
	if (ourSocket == -1) return nullptr;
	return new RTSPProxyServer(env, ourSocket, ourSocket, ourPort, authDatabase, reclamationSeconds);
}


void RTSPProxyServer::anonceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName)
{
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();

	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}

bool RTSPProxyServer::StartProxyServer(CString* inputURL, void* Data)
{
	TaskScheduler* newscheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*newscheduler);
	OutPacketBuffer::maxSize = 2000000;

	in_addr in_Addr;

	in_Addr.s_addr = chooseRandomIPv4SSMAddress(*env);
	unsigned short rtpPortNum = 18885;
	unsigned short rtcpPortNum = rtpPortNum + 1;

	Port rtpPort(rtpPortNum);
	Port rtcpPort(rtcpPortNum);
	unsigned const short ttl = 5000;
	Groupsock* rtpGS = new Groupsock(*env, *(sockaddr_storage*)&in_Addr, rtpPort, ttl);
	Groupsock* rtcpGS = new Groupsock(*env, *(sockaddr_storage*)&in_Addr, rtcpPort, ttl);

	rtpGS->multicastSendOnly();
	rtcpGS->multicastSendOnly();

	RTPSource* outSource = H264VideoRTPSource::createNew(*env, rtpGS, 96);
	H264VideoRTPSink* outputSink = H264VideoRTPSink::createNew(*env, rtpGS, 96);
	RTSPProxyServer* server = RTSPProxyServer::createNew(*env, 188554);

	const unsigned estimatedSessionBandwidth = 500;
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0';

	RTCPInstance* rtcp = RTCPInstance::createNew(*env, rtcpGS, estimatedSessionBandwidth, CNAME, outputSink, outSource, true);

	ServerMediaSession* sms = ServerMediaSession::createNew(*env, "retranslate"); 
	
#define _RTP_SINK_HH
	OnDemandServerMediaSubsession* proxy = H264VideoFileServerMediaSubsession::createNew(*env, "test.264", true);
	sms->addSubsession(proxy);													  
	server->addServerMediaSession(sms);
	server->registerStream(sms, "192.168.56.1:8000", 8000, HandlerForRegister);

	H264VideoStreamFramer* framer = H264VideoStreamFramer::createNew(*env, outputSink->source(), false);
	framer->flushInput();
	outputSink->startPlaying(*outSource, proxyServerMediaSubsessionAfterPlaying, sms);
	RTSPProxyServer::anonceStream(server, sms, "retranslate");

	env->taskScheduler().doEventLoop();
	return true;
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
}

void proxyServerMediaSubsessionAfterPlaying(void* clientData)
{
	RTSPServer* server = (RTSPServer*)clientData;
	MediaSubsession* subsession = (MediaSubsession*)clientData;

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