#include "NetSocketUV.hpp"

using std::ofstream;
ofstream fout;

static void decodeURL(char* url) {
	char* cursor = url;
	while (*cursor) {
		if ((cursor[0] == '%') &&
			cursor[1] && isxdigit(cursor[1]) &&
			cursor[2] && isxdigit(cursor[2])) {

			char hex[3];
			hex[0] = cursor[1];
			hex[1] = cursor[2];
			hex[2] = '\0';
			*url++ = (char)strtol(hex, NULL, 16);
			cursor += 3;
		}
		else {
			*url++ = *cursor++;
		}
	}

	*url = '\0';
}

NetSocketUV::NetSocketUV(Net* net) : NetSocket(net)
{
	sock = NULL;
	status = errno;
#ifndef WIN32
	sender = nullptr;
#endif // !WIN32
}

NetSocketUV::~NetSocketUV()
{
	Destroy();
	status = 0;
}

bool NetSocketUV::Create(int port, bool udp_tcp, bool listen, SOCKET_MODE mode)
{	
	NetSocket::Create(port, udp_tcp, listen, mode);
	uv_loop_t* loop = GetLoop(net);	
	if (udp_tcp)
	{
		sock = malloc(sizeof(TCP_SOCKET));
		memset(sock, 0, sizeof(TCP_SOCKET));
		((TCP_SOCKET*)sock)->net_socket = this;
		
		uv_tcp_t* tcp = GetPtrTCP(sock);
		int r = uv_tcp_init(loop, tcp);
		assert(r == 0);
		r = uv_tcp_nodelay(tcp, true);
		assert(r == 0);

		if (listen)
		{
			switch (mode)
			{
			case SOCKET_MODE_DEFAULT:
				break;
			case SOCKET_MODE_TCP:
			{
				sockaddr_in* sock_addres = new sockaddr_in;
				int i = uv_ip4_addr("0.0.0.0", port, sock_addres);
				assert(i == 0);
				int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
				assert(b == 0);
				int l = uv_listen((uv_stream_t*)tcp, 10240, OnAccept);
				if (l != 0)
					return false;
			}
				break;
			case SOCKET_MODE_RTSP:
			{
				sockaddr_in* sock_addres = new sockaddr_in;
				int i = uv_ip4_addr("0.0.0.0", port, sock_addres);
				assert(i == 0);
				int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
				assert(b == 0);
				int l = uv_listen((uv_stream_t*)tcp, 10240, OnAcceptRTSP);
				if (l != 0)
					return false;
			}
				break;
			default:
				break;
			}
		}		
		return true;
	}
	else
	{
		sock = malloc(sizeof(TCP_SOCKET));
		memset(sock, 0, sizeof(UDP_SOCKET));
		uv_udp_t* udp = GetPtrUDP(sock);
		int r = uv_udp_init(loop, udp);
		assert(r == 0);

		struct sockaddr_in broadcast_addr;
		uv_ip4_addr("0.0.0.0", port, &broadcast_addr);
		r = uv_udp_bind(udp, (const struct sockaddr*)&broadcast_addr, 0);
		assert(r == 0);

		r = uv_udp_set_broadcast(udp, 1);
		assert(r == 0);

		if (listen)
		{
			r = uv_udp_recv_start(udp, OnAllocBuffer, OnReadUDP);
			assert(r == 0);
		}

		((UDP_SOCKET*)sock)->net_socket = this;
		return true;
	}
	return false;
}

char address_converter[30];
static CString IParr[10240];
bool NetSocketUV::GetIP(CString& addr, bool own_or_peer)
{
	if (NetSocket::GetIP(addr, own_or_peer))
	{
		uv_tcp_t* socket = GetPtrTCP(sock);
		sockaddr sockName;
		memset(&sockName, -1, sizeof(sockName));
		int nameLen = sizeof(sockName);
		int r;
		
		if (own_or_peer)
			r = uv_tcp_getpeername(socket, &sockName, &nameLen);
		else
			r = uv_tcp_getsockname(socket, &sockName, &nameLen);

		if (r == 0)
		{
			r = uv_ip4_name((sockaddr_in*)&sockName, address_converter, sizeof(address_converter));
			if (r == 0)
			{
				addr = address_converter;
				
				unsigned char* port_ptr = (unsigned char*)&(((sockaddr_in*)&sockName)->sin_port);
				int port = port_ptr[1];
				port += port_ptr[0] << 8;
				addr += ':';
				CString d;
				d.IntToString(port);
				addr += d;
				//this->addr->ip = IParr[ClientID];
			}
		}
		else
		{
			printf("NetSocketUV::GetIP() error:\t%s\n", uv_strerror(r));
		}
	}
	return false;
}
int count = 1;
bool NetSocketUV::Accept()
{
	NetSocketUV* accept_sock = (NetSocketUV*)net->NewSocket(net);
	accept_sock->Create(0, true, false, SOCKET_MODE_DEFAULT);
	uv_tcp_t* host = GetPtrTCP(sock);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);
	if (uv_accept((uv_stream_t*)host, (uv_stream_t*)client) == 0)
	{	
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP) == 0)
		{
			accept_sock->GetIP(accept_sock->ip, Peer);
			ServerUV* server = ((ServerUV*)net);
			bool is_same = false;
			
			CString fileName;
			CString IDstr;
			IParr[server->count_accept] = accept_sock->ip;
			count++;
			if ((is_same = assertIP(IParr, accept_sock->ip)) == true || (count%3)!=0)
			{
				
				server->count_accept++;
				server->sockets_nohello.Add(accept_sock);
			}
			else
			{
				server->count_accept++;
				server->sockets_nohello.Add(accept_sock);
				server->ConnectSocket(accept_sock, server->count_accept);
				if (!accept_sock->sessionID)
				{
					NET_SERVER_SESSION* ss = new NET_SERVER_SESSION(server);
					server->AddSessionInfo(ss, accept_sock);
				}

				
			}
#ifndef WIN32

			if (!is_same || (count % 3) == 0)
			{
				fileName += "tcp://localhost:8554/";
				if (accept_sock->ClientID == 0)
					fileName += "0in_binary.264";
				else {
					IDstr.IntToString(accept_sock->ClientID);
					fileName += IDstr;
					fileName += "in_binary.264";
				}

				accept_sock->sender = FF_encoder::createNew(fileName.c_str());
				accept_sock->sender->SetupOutput();
			}

#endif // !WIN32			
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
	return false;
}

bool NetSocketUV::AcceptRTSP()
{
	NetSocketUV* accept_sock = (NetSocketUV*)net->NewSocket(net);
	accept_sock->Create(0, true, false, SOCKET_MODE_DEFAULT);
	uv_tcp_t* host = GetPtrTCP(sock);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);
	if (uv_accept((uv_stream_t*)host, (uv_stream_t*)client) == 0)
	{
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadRTSPcommands) == 0)
		{
			accept_sock->GetIP(accept_sock->ip, Peer);
			ServerUV* server = ((ServerUV*)net);
			
			server->count_accept++;
			server->sockets_nohello.Add(accept_sock);
			server->ConnectSocket(accept_sock, server->count_accept);
			if (!accept_sock->sessionID)
			{
				NET_SERVER_SESSION* ss = new NET_SERVER_SESSION(server);
				server->AddSessionInfo(ss, accept_sock);
			}
			printf("Accepted client with ID:%u\nIP:\t%s\nSessionID:\t%u\n\n", accept_sock->ClientID, accept_sock->ip.c_str(), accept_sock->sessionID);
			return true;
		}
	}
	return false;
}

void NetSocketUV::ReceiveRTSPcommand()
{
	NetBuffer* recv_buf = net->GetRecvBuffer();
	int received_bytes = recv_buf->GetLength();
	recvbuffer.Add(received_bytes, recv_buf->GetData());
	ServerUV* server = (ServerUV*)net;
	assert(server);
	server->fRequestBytesAlreadySeen++;
	server->fRequestBuffer[server->fRequestBytesAlreadySeen] = '\0';
	
	char cmdName[RTSP_PARAM_STRING_MAX];
	char urlPreSuffix[RTSP_PARAM_STRING_MAX];
	char urlSuffix[RTSP_PARAM_STRING_MAX];
	char cseq[RTSP_PARAM_STRING_MAX];
	char sessionIdStr[RTSP_PARAM_STRING_MAX];
	unsigned contentLength = 0;

	bool parseSuccess = server->parseRTSPRequestString((char*)server->fRequestBuffer, server->fLastCRLF + 2 - server->fRequestBuffer,
		cmdName, sizeof cmdName,
		urlPreSuffix, sizeof urlPreSuffix,
		urlSuffix, sizeof urlSuffix,
		cseq, sizeof cseq,
		sessionIdStr, sizeof sessionIdStr,
		contentLength, true);

	if (strcmp((const char*)recvbuffer.GetData(), "DESCRIBE"))
	{
		CString d;
		d.IntToString(ClientID);
		d += "in_binary.264";
		server->handleCmd_DESCRIBE("rtsp://", d.c_str(), "ServerRTSP/Play/");
	}
	else if (strcmp((const char*)recvbuffer.GetData(), "SETUP"))
	{

	}
	else if (strcmp((const char*)recvbuffer.GetData(), "PLAY"))
	{

	}
	else if (strcmp((const char*)recvbuffer.GetData(), "STOP"))
	{
		uv_read_stop((uv_stream_t*)GetPtrTCP(sock));
	}
}

void NetSocketUV::ReceiveTCP()
{
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	recvbuffer.Add(received_bytes, recv_buffer->GetData());

	int filePrefix = (int)ClientID;
	CString fileName;
	if (filePrefix == 0)
		fileName = "0in_binary.264";
	else
	{
		fileName.IntToString(filePrefix);
		fileName += "in_binary.264";
	}
	ReceiveMessages();

	fout.open(fileName.c_str(), std::ios::binary | std::ios::app);
	if (fout.is_open())
	{
		fout.write((char*)recvbuffer.GetData(), recvbuffer.GetLength());
		fout.close();
	}
	else
	{
		printf("cannot open file\n");
	}
	/*AVIOContext* ctx = avio_alloc_context(recvbuffer.GetData(), recvbuffer.GetLength(), 0, nullptr, nullptr, nullptr, nullptr);
	if (!ctx)
	{
		printf("ERROR\n");
	}
	sender->setAVIOCtx(ctx);*/
	//if (sender->accepted == true)
#ifndef WIN32
	sender->Write(sender->getOutAVIOCtx(), this);
#endif // !WIN32

	
	/*if (recvbuffer.GetPacketCount() > 0 && received_bytes > 5000)
	{
		sender->SetupInput(fileName);
		sender->Write(sender->getInFmtCtx(), sender->getOutFmtCtx());
		sender->CloseInput();
	}
	
	recvbuffer.IncrementPacket();*/
}

void NetSocketUV::ReceiveUPD()
{
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	recvbuffer.Add(received_bytes, recv_buffer->GetData());

	if (net->IsServer())
		ReceiveMessages();

	CString fileName;
	fileName.IntToString((int)ClientID);
	fileName += "in_binary.264";

	fout.open(fileName.c_str(), std::ios::binary | std::ios::app);
	if (fout.is_open())
	{
		fout.write((char*)net->GetRecvBuffer()->GetData(), net->GetRecvBuffer()->GetLength());
		//printf("writed %d bytes in file %u\n", (int)net->GetRecvBuffer()->GetLength(), fileName.c_str());
		fout.close();

	}
	else
	{
		printf("cannot open file\n");
	}

}

void NetSocketUV::SendTCP(NET_BUFFER_INDEX* buf)
{
	if (buf->GetLength() > 0)
	{
		uv_buf_t buffer;
		buffer.len = buf->GetLength();
		buffer.base = (char*)buf->GetData();
		int r = uv_write(((NetBufferUV*)buf)->GetPtrWrite(), (uv_stream_t*)GetPtrTCP(sock), &buffer, 1, OnWrite);
		if (r != 0)
		{
			printf("send error!\t%s\n", uv_strerror(r));
			exit(1);
		}
	}
}

void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const sockaddr* addr, unsigned flags)
{
	NetSocketUV* socket = (NetSocketUV*)GetNetSocketPtr(handle);
	printf("received %d bytes from client with ID: %d\n", nread, (int)socket->GetClientID());
	NetBuffer* recv_buffer = socket->getNet()->GetRecvBuffer();
	assert(buf->base == (char*)recv_buffer->GetData());
	recv_buffer->SetLength(nread);

	/*size_t addrSize = sizeof addr;
	int r = uv_ip4_name((sockaddr_in*)&addr, address_converter, addrSize);
	socket->getAddr()->ip = address_converter;
	unsigned char* port_ptr = (unsigned char*)&(((sockaddr_in*)addr)->sin_port);
	socket->getAddr()->port = port_ptr[1];
	socket->getAddr()->port += port_ptr[0] << 8;*/

	socket->ReceiveUPD();
}

void OnReadTCP(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	NetSocketUV* uvsocket = (NetSocketUV*)GetNetSocketPtr(stream);
	if (nread < 0)
	{
		printf("Client disconnected!\n");
#ifndef WIN32
		uvsocket->sender->CloseOutput();
#endif // !WIN32
		uvsocket->getNet()->OnLostConnection(uvsocket);
		//OnCloseSocket((uv_handle_t*)stream);
		
	}
	else
	{
		//printf("Reading data from client with ID: %u\n", uvsocket->GetClientID());
		NetBuffer* recv_buff = uvsocket->getNet()->GetRecvBuffer();
		assert(buf->base == (char*)recv_buff->GetData());
		recv_buff->SetMaxSize(nread);
		uvsocket->ReceiveTCP();
	}
}

void OnAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(handle);
	NetBuffer* recv_buffer = net_sock->getNet()->GetRecvBuffer();

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
	
	list->DeleteBuffer(index);
}

void OnAccept(uv_stream_t* stream, int status)
{
	std::cout << "___ On Connect ___" << std::endl;
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(stream);
	net_sock->Accept();
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
		sock = NULL;
	}
	NetSocket::Destroy();
}

void SetupRetranslation(void* net, CString fileName)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	NetSocketUV* sock = (NetSocketUV*)net;
	/*CString IP_str;
	CString outURL("rtp://");
	sock->GetIP(IP_str, Owner);
	outURL += IP_str;
	outURL += "/";
	if (sock->GetClientID() == 0)
		outURL += "0in_binary.264";
	else
	{
		outURL += (int)sock->GetClientID();
		outURL += "in_binary.264";
	}

	if (sock->GetClientID() == 0)
		fileName = "0in_binary.264";*/

	//printf("input file name: %s\n output URL: %s\n", fileName.c_str(), outURL.c_str());
#ifndef WIN32
	sock->sender = FF_encoder::createNew(fileName.c_str()/*outURL.c_str()*/);
	sock->sender->SetupOutput();
#endif // !WIN32
	//while (true)
	//{
	//	sock->sendersender->Write(/*sender->getInFmtCtx(), */sender->getOutAVIOCtx(), sock);
	//	sock->sendersender->CloseInput();
	//	sock->sendersender->SetupInput(fileName);
	//}
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	/*CString command;
	command += "ffmpeg -i ";
	command += fileName; command += " tcp://";
	CString IP;
	accept_sock->GetIP(IP, Owner);
	IP += "/";
	IP += fileName;

	command += " "; command += IP;
	printf("command: %s\n", command.c_str());

	system(command.c_str());*/
//	std::cout << "thrad id: " << std::this_thread::get_id() << std::endl;
//	assert(client);
//	if (client->GetClientID() >= 0)
//	{
//		/*if (ID == 12499)
//			ID = 0;*/
//		/*CString IDstr(ID);
//		IDstr += "in_binary_h.264";*/
//		/*std::vector<char> strID;
//		std::to_chars(strID.data(), strID.data() + strID.size(), clientID);*/
//		/*CString IDStr;
//		if(client->GetClientID() == 0)
//			IDStr.IntToString(0);
//		else
//			IDStr = (char*)client->GetClientID();
//		IDStr += "in_binary.h264";*/
//		if (std::filesystem::exists((std::string)fileName) == true) {
//			
//			printf("Client file reading %s\n", fileName.c_str());
//			FILE* proxy = nullptr;
//#ifdef WIN32
//			//system("RTSPProxyServerForClient.exe -d -c -%s");
//			proxy = _popen("RTSP.exe -d -c -%s", "r");
//			_pclose(proxy);		
//			
//#else
//			int status;
//			pid_t pid;
//
//			pid = fork();
//			std::string outRTSP;
//			/* Handeling Chile Process */
//			std::cout << pid << std::endl;
//			if (pid == 0) {
//				//char* execv_str[] = { "./RTSP", (char*)IDStr.c_str(), NULL};
//				
//				if (execl("./RTSP", "./RTSP", fileName.c_str(), NULL) < 0) {
//					status = -1;
//					perror("ERROR\n");
//				}
//				else
//				{
//					printf("Start translation success!\n");
//					std::getline(std::cin, outRTSP);
//					if (outRTSP.find("rtsp://"))
//					{
//						std::thread delay(NetSocketUV::WaitingDelay, &client);
//						//delay.join();
//						delay.detach();
//						//kill(pid, 0);
//					}
//				}
//			}
//			/* Handeling Chile Process Failure */
//			else if (pid < 0) {
//				status = -1;
//				perror("ERROR\n");
//			}
//#endif
//		}
//		else
//		{
//			printf("Cannot find client's file with ID: %s!", fileName.c_str());
//			exit(1);
//		}
//	}
	return;
}

void OnAcceptRTSP(uv_stream_t* stream, int status)
{
	std::cout << "___ On Connect ___" << std::endl;
	if (status < 0)
	{
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	NetSocketUV* net_sock = (NetSocketUV*)GetNetSocketPtr(stream);
	net_sock->AcceptRTSP();
}

bool ServerUV::parseRTSPRequestString(char const* reqStr, unsigned reqStrSize, char* resultCmdName, unsigned resultCmdNameMaxSize, char* resultURLPreSuffix, unsigned resultURLPreSuffixMaxSize, char* resultURLSuffix, unsigned resultURLSuffixMaxSize, char* resultCSeq, unsigned resultCSeqMaxSize, char* resultSessionIdStr, unsigned resultSessionIdStrMaxSize, unsigned& contentLength, bool urlIsRTSPS)
{
	urlIsRTSPS = false;

	unsigned i;
	for (i = 0; i < reqStrSize; ++i) {
		char c = reqStr[i];
		if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\0')) break;
	}
	if (i == reqStrSize) return false;

	bool parseSucceeded = false;
	unsigned i1 = 0;
	for (; i1 < resultCmdNameMaxSize - 1 && i < reqStrSize; ++i, ++i1) {
		char c = reqStr[i];
		if (c == ' ' || c == '\t') {
			parseSucceeded = true;
			break;
		}

		resultCmdName[i1] = c;
	}
	resultCmdName[i1] = '\0';
	if (!parseSucceeded) return false;

	unsigned j = i + 1;
	while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t')) ++j;
	for (; (int)j < (int)(reqStrSize - 8); ++j) {
		if ((reqStr[j] == 'r' || reqStr[j] == 'R')
			&& (reqStr[j + 1] == 't' || reqStr[j + 1] == 'T')
			&& (reqStr[j + 2] == 's' || reqStr[j + 2] == 'S')
			&& (reqStr[j + 3] == 'p' || reqStr[j + 3] == 'P')) {
			if (reqStr[j + 4] == 's' || reqStr[j + 4] == 'S') {
				urlIsRTSPS = true;
				++j;
			}
			if (reqStr[j + 4] == ':' && reqStr[j + 5] == '/') {
				j += 6;
				if (reqStr[j] == '/') {
					++j;
					while (j < reqStrSize && reqStr[j] != '/' && reqStr[j] != ' ') ++j;
				}
				else {
					--j;
				}
				i = j;
				break;
			}
		}
	}

	parseSucceeded = false;
	for (unsigned k = i + 1; (int)k < (int)(reqStrSize - 5); ++k) {
		if (reqStr[k] == 'R' && reqStr[k + 1] == 'T' &&
			reqStr[k + 2] == 'S' && reqStr[k + 3] == 'P' && reqStr[k + 4] == '/') {
			while (--k >= i && reqStr[k] == ' ') {}
			unsigned k1 = k;
			while (k1 > i && reqStr[k1] != '/') --k1;

			unsigned n = 0, k2 = k1 + 1;
			if (k2 <= k) {
				if (k - k1 + 1 > resultURLSuffixMaxSize) return false;
				while (k2 <= k) resultURLSuffix[n++] = reqStr[k2++];
			}
			resultURLSuffix[n] = '\0';

			n = 0; k2 = i + 1;
			if (k2 + 1 <= k1) {
				if (k1 - i > resultURLPreSuffixMaxSize) return false;
				while (k2 <= k1 - 1) resultURLPreSuffix[n++] = reqStr[k2++];
			}
			resultURLPreSuffix[n] = '\0';
			decodeURL(resultURLPreSuffix);

			i = k + 7;
			parseSucceeded = true;
			break;
		}
	}
	if (!parseSucceeded) return false;

	parseSucceeded = false;
	for (j = i; (int)j < (int)(reqStrSize - 5); ++j) {
		if (_strncasecmp("CSeq:", &reqStr[j], 5) == 0) {
			j += 5;
			while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t')) ++j;
			unsigned n;
			for (n = 0; n < resultCSeqMaxSize - 1 && j < reqStrSize; ++n, ++j) {
				char c = reqStr[j];
				if (c == '\r' || c == '\n') {
					parseSucceeded = true;
					break;
				}

				resultCSeq[n] = c;
			}
			resultCSeq[n] = '\0';
			break;
		}
	}
	if (!parseSucceeded) return false;

	for (j = i; (int)j < (int)(reqStrSize - 8); ++j) {
		if (_strncasecmp("Session:", &reqStr[j], 8) == 0) {
			j += 8;
			while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t')) ++j;
			unsigned n;
			for (n = 0; n < resultSessionIdStrMaxSize - 1 && j < reqStrSize; ++n, ++j) {
				char c = reqStr[j];
				if (c == '\r' || c == '\n') {
					break;
				}

				resultSessionIdStr[n] = c;
			}
			resultSessionIdStr[n] = '\0';
			break;
		}
	}

	// Also: Look for "Content-Length:" (optional, case insensitive)
	contentLength = 0; // default value
	for (j = i; (int)j < (int)(reqStrSize - 15); ++j) {
		if (_strncasecmp("Content-Length:", &(reqStr[j]), 15) == 0) {
			j += 15;
			while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t')) ++j;
			unsigned num;
			if (sscanf(&reqStr[j], "%u", &num) == 1) {
				contentLength = num;
			}
		}
	}
	return true;
}

void ServerUV::handleCmd_DESCRIBE(char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr)
{
	char urlTotalSuffix[2 * RTSP_PARAM_STRING_MAX];
	
	urlTotalSuffix[0] = '\0';
	if (urlPreSuffix[0] != '\0') {
		strcat(urlTotalSuffix, urlPreSuffix);
		strcat(urlTotalSuffix, "/");
	}
	strcat(urlTotalSuffix, urlSuffix);

}

void ServerUV::handleCmd_DESCRIBE_afterLookup(NET_SERVER_SESSION* sess)
{
	char* sdpDescription = NULL;
	char* rtspURL = NULL;
	do {
		if (sess == NULL) {
			handleCmd_notFound();
			break;
		}

		sess->incrementReferenceCount();

		sdpDescription = sess->generateSDPDescription(AF_INET);
		if (sdpDescription == NULL) {
			setRTSPResponse("404 File Not Found, Or In Incorrect Format");
			break;
		}
		unsigned sdpDescriptionSize = strlen(sdpDescription);
		CString rtspURL;
		rtspURL += "rtsp://";
		NetSocketUV* sock = (NetSocketUV*)sockets.Get(count_accept);
		sock->GetIP(rtspURL, Peer);
		rtspURL.Delete(rtspURL.Find(":", 6));
		CString d;
		d.IntToString(sock->ClientID);
		rtspURL += d;
		rtspURL += "in_binary.264";

		snprintf((char*)fResponseBuffer, sizeof fResponseBuffer,
			"RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
			"%s"
			"Content-Base: %s/\r\n"
			"Content-Type: application/sdp\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s",
			fCurrentCSeq,
			dateHeader(),
			rtspURL.c_str(),
			sdpDescriptionSize,
			sdpDescription);
	} while (0);

	//if (sess != NULL) {
	//	// Decrement its reference count, now that we're done using it:
	//	sess->decrementReferenceCount();
	//	if (sess->referenceCount() == 0 && sess->deleteWhenUnreferenced()) {
	//		fOurServer.removeServerMediaSession(session);
	//	}
	//}

	delete[] sdpDescription;
	delete[] rtspURL;
}

void ServerUV::handleCmd_notFound()
{
	setRTSPResponse("404 Stream Not Found");
}

void ServerUV::setRTSPResponse(const char* responseStr)
{
	snprintf((char*)fResponseBuffer, sizeof fResponseBuffer,
		"RTSP/1.0 %s\r\n"
		"CSeq: %s\r\n"
		"%s\r\n",
		responseStr,
		fCurrentCSeq,
		dateHeader());
}

void OnReadRTSPcommands(uv_stream_t* handle, ssize_t suggested_size, const uv_buf_t* buf)
{
	NetSocketUV* socket = (NetSocketUV*)GetNetSocketPtr(handle);
	if (suggested_size < 0)
	{
		socket->getNet()->OnLostConnection(socket);
	}
	NetBuffer* recv_buffer = socket->getNet()->GetRecvBuffer();
	assert(buf->base == (char*)recv_buffer->GetData());
	recv_buffer->SetMaxSize(suggested_size);
	socket->ReceiveRTSPcommand();
}

void* NetSocketUV::WaitingDelay(void* delay)
{
	
	if (delay != nullptr)
	{
		std::this_thread::sleep_for(std::chrono::minutes(10));
		NetSocketUV* socket = (NetSocketUV*)delay;
		socket->Destroy();
		return nullptr;
	}
	else
	{
		printf("delay <= 0!");
		return 0;
	}
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
	ServerUV* serv = (ServerUV*)net;
	return &(serv->loop);
}

unsigned int timer_count = 0;
ServerUV* uv_server = NULL;
ServerUV::ServerUV()
{
	timer_count = 0;
	memset(&loop, 0, sizeof(loop));

	int r = uv_loop_init(&loop);
	assert(r == 0);
	uv_server = this;
	fRequestBytesAlreadySeen = 0;
}

ServerUV::~ServerUV()
{
	Destroy();
}

void ServerUV::Destroy()
{
	Server::Destroy();
	uv_server = NULL;

	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
}

void ServerUV::StopServer()
{
	uv_stop(&loop);
}

NetSocket* ServerUV::NewSocket(Net* net)
{
	return new NetSocketUV(net);
}

NET_BUFFER_INDEX* ServerUV::NewBuffer(int index)
{
	return new NetBufferUV(index);
}

void ServerUV::StartUVServer(bool internet)
{
	//StopServer();
	if (internet)
	{
		printf("What a protocol should been used: UDP or TCP?\n");
		char* str = new char[4];
		std::string proto;
		scanf("%s", str);
		proto = str;
		if (proto.find("UDP") != -1)
			udp_tcp = false;
		else
			udp_tcp = true;
		bool res = Create(udp_tcp);
		if (res)
		{
			printf("Success create server\n"/*, UDP is: %s", udp_tcp ? "true" : "false"*/);
			UpdateNet();
		}
	}
}

bool ServerUV::UpdateNet()
{
	uv_loop_t* loop = GetLoop(this);

	uv_run(loop, UV_RUN_DEFAULT);
	return false;
}
