#include "NetSocketUV.hpp"

using std::ofstream;
ofstream fout;

NetSocketUV::NetSocketUV(Net* net) : NetSocket(net)
{
	sock = NULL;
	status = errno;
}

NetSocketUV::~NetSocketUV()
{
	Destroy();
	status = 0;
}

bool NetSocketUV::Create(int port, bool udp_tcp, bool listen)
{	
	NetSocket::Create(port, udp_tcp, listen);
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
			sockaddr_in* sock_addres = new sockaddr_in;
			int i = uv_ip4_addr("0.0.0.0", port, sock_addres);
			assert(i == 0);
			int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
			assert(b == 0);
			int l = uv_listen((uv_stream_t*)tcp, 10240, OnAccept);
			if (l != 0)
				return false;
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
				
				
			}
		}
		else
		{
			printf("NetSocketUV::GetIP() error:\t%s\n", uv_strerror(r));
		}
	}
	return false;
}

bool NetSocketUV::Accept()
{
	NetSocketUV* accept_sock = (NetSocketUV*)net->NewSocket(net);
	accept_sock->Create(0, true, false);
	uv_tcp_t* host = GetPtrTCP(sock);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);
	if (uv_accept((uv_stream_t*)host, (uv_stream_t*)client) == 0)
	{	
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP) == 0)
		{
			accept_sock->GetIP(accept_sock->ip, Peer);
			CMemWriter* wr1 = net->getWR1();
			ServerUV* server = ((ServerUV*)net);
			if (assertIP(IParr))
			{
				printf("assertIP(%p) return true\n");
				int sessID = accept_sock->sessionID++;
				(*wr1) << sessID;
				server->sockets_nohello.Add(accept_sock);
				NET_SESSION_INFO* ss = new NET_SESSION_INFO(net);
				assert(ss);
				server->AddSessionInfo(ss, accept_sock);
			}
			else
			{
				server->count_accept++;
				accept_sock->sessionID++;
				(*wr1).Start();
				(*wr1) << (accept_sock->ip);
				MEM_DATA buf;
				(*wr1).Finish(buf);

				if (server->sockets_nohello.Add(accept_sock))
				{
					NET_SESSION_INFO* ss = new NET_SESSION_INFO(net);
					assert(ss);
					ss->Serialize(*wr1);
					server->AddSessionInfo(ss, accept_sock);
					server->ConnectSocket(accept_sock, server->count_accept);
				}
			}
			CString fileName;
			if(accept_sock->ClientID == 0)
				fileName += "0in_binary.264";
			else {
				fileName += (int)accept_sock->ClientID;
				fileName += "in_binary.264";
			}
			
			//FF_encoder* sender = FF_encoder::createNew(accept_sock->ip.c_str(), fileName);
			/*std::thread RTSPsend(SetupRetranslation, accept_sock, fileName);
			RTSPsend.detach();*/
			accept_sock->sender = FF_encoder::createNew("rtsp://192.168.0.69:8554/serverPlay/");
			printf("Accepted client with ID:%u\nIP:\t%s\nSessionID:\t%u\n\n", accept_sock->ClientID, accept_sock->ip.c_str(), accept_sock->sessionID);
			
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
		//printf("writed %d bytes in file %s\n", (int)net->GetRecvBuffer()->GetLength(), fileName.c_str());
		fout.close();
	}
	else
	{
		printf("cannot open file\n");
	}
	
	if (recvbuffer.GetPacketCount() > 0 && received_bytes > 3000)
	{
		sender->SetupInput(fileName);
		sender->Write();
		sender->CloseInput();
	}
	recvbuffer.IncrementPacket();

	
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

void StartReadingThread(void* handle)
{
	uv_read_start((uv_stream_t*)handle, OnAllocBuffer, OnReadTCP);
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
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	NetSocketUV* sock = (NetSocketUV*)net;
	CString IP_str;
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
		fileName = "0in_binary.264";

	printf("input file name: %s\n output URL: %s\n", fileName.c_str(), outURL.c_str());
	/*FF_encoder* sender = FF_encoder::createNew(outURL.c_str(), fileName);
	sender->Write();*/

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
