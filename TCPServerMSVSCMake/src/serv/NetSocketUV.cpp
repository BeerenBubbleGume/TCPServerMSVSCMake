#include "NetSocketUV.hpp"

FILE* outH264 = fopen("h.264", "w");

NetSocketUV::NetSocketUV(Net* net) : NetSocket(net)
{
	sock = NULL;
	status = errno;	
	loop = uv_default_loop();
	
}

void serverBYEHandler(void* instance, u_int8_t requestBytes)
{
	RTCPInstance* rtcp= (RTCPInstance*)instance;
		
}

void proxyServerMediaSubsessionAfterPlaying(void* clientData)
{
	RTSPServer* server = (RTSPServer*)clientData;
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(clientData);
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

void HandlerForRegister(void* clientData)
{
}

NetSocketUV::~NetSocketUV()
{
	status = 0;
}

bool NetSocketUV::Create(int port, bool udp_tcp, bool listen)
{	
	NetSocket::Create(port, udp_tcp, listen);
	uv_loop_t* sloop = GetLoop(net);
	if (sloop)
	{
		
		if (udp_tcp)
		{
			sock = new TCP_SOCKET;
			memset(sock, 0, sizeof(TCP_SOCKET));
			((TCP_SOCKET*)sock)->net_socket = this;
			
			uv_tcp_t* tcp = GetPtrTCP(sock);

			int r = uv_tcp_init(loop, tcp);
			assert(r == 0);

			r = uv_tcp_nodelay(tcp, true);
			assert(r == 0);

			if (listen)
			{
				sockaddr_in* sock_addres = new sockaddr_in;
				
				int i = uv_ip4_addr("127.0.0.1", 188554, sock_addres);
				assert(i == 0);
				int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
				assert(b == 0);
				int l = uv_listen((uv_stream_t*)tcp, 1024, OnAccept);
				if (l)
					return false;
				else
					return uv_run(sloop, UV_RUN_DEFAULT);
			}
				
		}
		else
		{
			sock = malloc(sizeof(UDP_SOCKET));
			memset(sock, 0, sizeof(UDP_SOCKET));
			int r = uv_udp_init(loop, GetPtrUDP(sock));
			assert(r == 0);

			struct sockaddr_in broadcast_addr;
			uv_ip4_addr(addr->address, addr->port, &broadcast_addr);
			r = uv_udp_bind(GetPtrUDP(sock), (const struct sockaddr*)&broadcast_addr, 0);
			assert(r == 0);

			r = uv_udp_set_broadcast(GetPtrUDP(sock), 1);
			assert(r == 0);

			if (listen)
			{
				r = uv_udp_recv_start(GetPtrUDP(sock), OnAllocBuffer, OnReadUDP);
				assert(r == 0);
			}

			((UDP_SOCKET*)sock)->net_socket = this;
		}
		return true;
	}
	else
	{
		std::cout << "Loop cannot been initialized!" << std::endl;
		return false;
	}
}

bool NetSocketUV::GetIP(Net_Address* addr, bool own_or_peer)
{
	if (own_or_peer)
	{
		std::cout << "Set IP to socket!" << std::endl;
		udp_tcp = true;

	}
	else
		return false;
}

bool NetSocketUV::Accept(uv_tcp_t* handle)
{
	NetSocketUV* accept_sock = NewSocket(net);
	accept_sock->Create(554, true, false);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);

	if (uv_accept((uv_stream_t*)handle, (uv_stream_t*)client) == 0)
	{
		sockaddr sockname;
		int socklen = sizeof accept_sock->net->GetConnectSockaddr();
		int curID = uv_tcp_getsockname(client, &accept_sock->net->GetConnectSockaddr(), &socklen);
		accept_sock->SetID(client);
		std::cout << "Start accepting RTSP from: " << curID << std::endl;
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP) == 0)
		{
			return true;
		}
		else
			return false;
	}
	else
	{
		std::cout << "Cannot accepting client!" << std::endl;
		return false;
	}
}

void NetSocketUV::SendTCP(NET_BUFFER_INDEX *buf)
{
	if (buf->GetLength() > 0)
	{
		uv_buf_t buffer;
		buffer.len = buf->GetLength();
		buffer.base = (char*)buf->GetData();
		uv_buf_init(buffer.base, buffer.len);
		int r = uv_write(((NetBufferUV*)buf)->GetPtrWrite(), (uv_stream_t*)GetPtrTCP(sock), &buffer, 1, OnWrite);
		assert(r == 0);
	}
}

void NetSocketUV::SendUDP(NET_BUFFER_INDEX *buf)
{
}

void NetSocketUV::ReceiveTCP()
{
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	//recv_buffer->Add(received_bytes, (void*)recv_buffer->GetData());
	NET_BUFFER_INDEX* index = net->PrepareMessage(net->GetIDPath(), received_bytes, recv_buffer->GetData());
	SendTCP(index);
}

void NetSocketUV::ReceiveUPD()
{
}

void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(stream);
	uvsocket->net->setupReceivingSocket(*uvsocket);
	std::cout << "Receiving..." << std::endl;
	if (nread < 0)
	{
		std::cout << "read buff < 0" << std::endl;
		uvsocket->net->OnLostConnection(uvsocket);
		OnCloseSocket((uv_handle_t*)stream);
	}
	else
	{
		

		std::cout << "Reading UV socket from client with ID:" << uvsocket->GetClientID() << std::endl;
		NetBuffer* recv_buff = uvsocket->net->GetRecvBuffer();
		recv_buff->SetMaxSize(nread);
		uvsocket->ReceiveTCP();		
	}
}

void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = net_sock->net->GetRecvBuffer();

	unsigned int max_length = recv_buffer->GetMaxSize();
	if (max_length < suggested_size)
		recv_buffer->SetMaxSize(suggested_size);
	buf->len = suggested_size;
	buf->base = (char*)recv_buffer->GetData();
}

void OnCloseSocket(uv_handle_t *handle)
{
	char* ptr = (char*)handle;
	ptr -= sizeof(void*);
	free(ptr);
}

void OnWrite(uv_write_t *req, int status)
{
	int offset = offsetof(NetBufferUV, sender_object);
	NetBufferUV* buf = (NetBufferUV*)(((char*)req) - offset);
	NET_BUFFER_LIST* list = (NET_BUFFER_LIST*)buf->owner;
	int index = buf->GetIndex(); 
	NetSocketUV* uvsocket = (NetSocketUV*)list->net->getReceivingSocket();

	std::ofstream outFile("H.264", std::ios::binary);
	for (int i = 0; i < offset; ++i)
		outFile.write((const char*)buf->GetData(), offset);
	outFile.close();
	uv_thread_t proxyThread;
	uv_thread_create(&proxyThread, NetSocketUV::GenerateRTSPURL, uvsocket);
	uv_thread_join(&proxyThread);

	list->DeleteBuffer(index);
	
}
void AfterCreateFile(uv_fs_t* req)
{
	uv_file h264 = 1;
	req->file.fd = h264;
	uv_buf_t buffer;
	
	int offset = offsetof(NetBufferUV, sender_object);
	NetBufferUV* buf = (NetBufferUV*)(((char*)req) - offset);
	NET_BUFFER_LIST* list = (NET_BUFFER_LIST*)buf->owner;
	int index = buf->GetIndex();
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(req);

	uv_fs_write(req->loop, req, h264, &buffer, 1, UV_FS_O_CREAT, OnWriteFile);
	list->DeleteBuffer(index);
}
void OnWriteFile(uv_fs_t* req)
{
	NetSocketUV* uvsocket = (NetSocketUV*)req->data;
	uvsocket->GenerateRTSPURL(uvsocket);	
}

char address_converter[30];
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
	NetSocket* socket = GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = socket->net->GetRecvBuffer();
	assert(buf->base == (char*)recv_buffer->GetData());
	recv_buffer->SetLength(nread);

	int r = uv_ip4_name((sockaddr_in*)addr, address_converter, sizeof(address_converter));
	socket->net->addr->address = address_converter;
	unsigned char* port_ptr = (unsigned char*)&(((sockaddr_in*)addr)->sin_port);
	socket->net->addr->port = port_ptr[1];
	socket->net->addr->port += port_ptr[0] << 8;

	socket->ReceiveUPD();
}

void OnAccept(uv_stream_t* stream, int status)
{
	std::cout << "___ On Connetc ___" << std::endl;
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	uv_thread_t connectThread;
	
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(stream);
	net_sock->Accept((uv_tcp_t*)stream);
}

void NetSocketUV::Destroy()
{
	if (sock)
	{
		if (udp_tcp)
		{
			uv_close((uv_handle_t *)GetPtrTCP(sock), OnCloseSocket);
			((TCP_SOCKET *)sock)->net_socket = nullptr;
		}
		else
		{
			int r = uv_read_stop((uv_stream_t *)GetPtrUDP(sock));
			assert(r == 0);
			uv_close((uv_handle_t *)sock, OnCloseSocket);
			((UDP_SOCKET *)sock)->net_socket = NULL;
		}
		sock = NULL;
	}
	NetSocket::Destroy();
}

NetSocketUV::RTSPProxyServer* NetSocketUV::RTSPProxyServer::createNew(UsageEnvironment& env, Port ourPort, UserAuthenticationDatabase* authDatabase, unsigned reclamationSeconds)
{
	int ourSocket4 = setUpOurSocket(env, ourPort, AF_INET);
	if (ourSocket4 == -1) return nullptr;
	int ourSocket6 = setUpOurSocket(env, ourPort, AF_INET6);
	if (ourSocket6 == -1) return nullptr;
	return new NetSocketUV::RTSPProxyServer(env, ourSocket4, ourSocket6, ourPort, authDatabase, reclamationSeconds);
}


void NetSocketUV::RTSPProxyServer::anonceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName)
{
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();

	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}

void NetSocketUV::RTSPProxyServer::StartProxyServer(/*CString* inputURL, */void* Data)
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

	//rtpGS->multicastSendOnly();
	//rtcpGS->multicastSendOnly();

	ByteStreamFileSource* outSource = ByteStreamFileSource::createNew(*env, "h.264");
	const unsigned estimatedSessionBandwidth = 500;
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0';
					
	RTPSink* outputSink = H264VideoRTPSink::createNew(*env, rtpGS, 96);
	//RTCPInstance* rtcp = RTCPInstance::createNew(*env, rtcpGS, estimatedSessionBandwidth, CNAME, outputSink, outSource, true);

	RTSPProxyServer* server = RTSPProxyServer::createNew(*env, 8554);	
	
	const char* streamName = "ServerMedia/"/* + *socket->GetClientID()*/;
														
	ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName);
	OnDemandServerMediaSubsession* proxy = NetSocketUV::DemandServerMediaSubsession::createNew(/*socket->net, */*env, true);
	sms->addSubsession(proxy);
	server->addServerMediaSession(sms);
	BasicUDPSink* UDPsink = BasicUDPSink::createNew(*env, rtpGS);

	H264VideoStreamFramer* framer = H264VideoStreamFramer::createNew(*env, outSource, false);
	framer->flushInput();
	outputSink->startPlaying(*outSource, proxyServerMediaSubsessionAfterPlaying, sms);
	RTSPProxyServer::anonceStream(server, sms, "retranslate");
	
	env->taskScheduler().doEventLoop();
	//return;
}

bool NetSocketUV::RTSPProxyServer::StopProxyServer(void* clientData)
{
	return false;
}

NetSocketUV::RTSPProxyServer::~RTSPProxyServer()
{
}

NetSocketUV::RTSPProxyServer::RTSPProxyServer(UsageEnvironment& env,
	int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
	UserAuthenticationDatabase* authDatabase,
	unsigned reclamationSeconds)
	: RTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort, authDatabase, reclamationSeconds)
{
}

uv_tcp_t *GetPtrTCP(void *ptr)
{
	return (uv_tcp_t*)(((char*)ptr) + sizeof(void*));
}

uv_udp_t *GetPtrUDP(void *ptr)
{
	return (uv_udp_t *)(((char *)ptr) + sizeof(void *));
}

uv_loop_t *GetLoop(Net* net)
{
	NetSocketUV serv = (NetSocketUV)net;
	return (serv.loop);
}

NetSocketUV::DemandServerMediaSubsession* NetSocketUV::DemandServerMediaSubsession::createNew(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource)
{
	return new DemandServerMediaSubsession(/*net, */env, reuseFirstSource);
}

NetSocketUV::DemandServerMediaSubsession::~DemandServerMediaSubsession()
{
	//if (fBuffer)
	//{
	//	delete[] fBuffer;
	//	//fBuffer = nullptr;
	//}
	//fBufferSize = 0;
}

#define NEW_SMS(description) do{\
const char* destStr = description\
", streamed by ProxyServer";\
sms = ServerMediaSession::createNew(env, fileName, fileName, destStr);\
}while(0)

ServerMediaSession* NetSocketUV::DemandServerMediaSubsession::createNewSMS(UsageEnvironment& env, const char* fileName, FILE* fid)
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

FramedSource* NetSocketUV::DemandServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	/*NetBuffer* recvBuff = net->GetRecvBuffer();
	fBuffer = recvBuff->GetData();
	fBufferSize = recvBuff->GetLength();*/

	FILE* in = fopen("h.264", "r");

	ByteStreamFileSource* fileSource = ByteStreamFileSource::createNew(envir(), in);
	if (fileSource == NULL) return NULL;
	fBufferSize = fileSource->fileSize();
		   
	// Create a framer for the Video Elementary Stream:
	return H264VideoStreamFramer::createNew(envir(), fileSource);
}

RTPSink* NetSocketUV::DemandServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
	return SimpleRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 9000, "video", "H264", 1, True, True);
}

void NetSocketUV::DemandServerMediaSubsession::subsessionByeHandler(void* clientData)
{
}

void NetSocketUV::DemandServerMediaSubsession::subsessionByeHandler()
{
}

NetSocketUV::DemandServerMediaSubsession::DemandServerMediaSubsession(/*Net* net, */UsageEnvironment& env, Boolean reuseFirstSource): OnDemandServerMediaSubsession(env, reuseFirstSource)/*, net(net)*/
{
	fBufferSize = 100000000;
	fBuffer = new u_int8_t[fBufferSize];
}