#include "NetSocketUV.hpp"
#pragma comment(lib, "../../libs/lib/uv.lib")

NetBufferUV::~NetBufferUV()
{
	index = NULL;
}

uv_write_t* NetBufferUV::GetPtrWrite()
{
	return (uv_write_t*)sender_object;
}

uv_udp_send_t* NetBufferUV::GetPtrSend()
{
	return (uv_udp_send_t*)sender_object;
}

NetSocketUV::NetSocketUV(Net* net) : NetSocket(net)
{
	sock = NULL;
	status = errno;	
	loop = uv_default_loop();
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
				
				int i = uv_ip4_addr("127.0.0.1", port, sock_addres);
				assert(i == 0);
				int b = uv_tcp_bind(tcp, (sockaddr*)sock_addres, 0);
				assert(b == 0);
				uv_thread_t acceptingThread;
				int l = uv_thread_create(&acceptingThread, OnListining, tcp);
				assert(l == 0);
				l = uv_thread_join(&acceptingThread);
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

const char* NetSocketUV::GetIP(Net_Address* addr, bool own_or_peer)
{
	if (own_or_peer)
	{
		std::cout << "Set IP to socket!" << std::endl;
		udp_tcp = true;
		struct addrinfo hints, *info, *p;
		int gai_result;

		char hostname[1024];
		hostname[1023] = '\0';
		gethostname(hostname, 1023);

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_CANONNAME;
		if ((gai_result = getaddrinfo(hostname, "rtsp", &hints, &info)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
			exit(1);
		}

		for (p = info; p != NULL; p = p->ai_next) {
			printf("hostname: %s\n", p->ai_canonname);
		}
		

		freeaddrinfo(info);
	}
	else
		return nullptr;
}

bool NetSocketUV::Accept(uv_handle_t* handle)
{
	NetSocketUV* accept_sock = NewSocket(net);
	accept_sock->Create(554, true, false);
	uv_tcp_t* client = GetPtrTCP(accept_sock->sock);

	if (uv_accept((uv_stream_t*)handle, (uv_stream_t*)client) == 0)
	{
		sockaddr sockname;
		int socklen = sizeof accept_sock->net->GetConnectSockaddr();
		//int curID = uv_tcp_getsockname(client, &accept_sock->net->GetConnectSockaddr(), &socklen);
		accept_sock->SetID(client);
		//std::cout << "Start accepting RTSP from: " << curID << std::endl;
		if (uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP) == 0)
		{	
			return true;
		}
		
	}
	else
	{
		std::cout << "Cannot accepting client!" << std::endl;
		return false;
	}
	return false;
}

void NetSocketUV::SendTCP(NET_BUFFER_INDEX *buf)
{
	if (buf->GetLength() > 0)
	{
		uv_buf_t buffer;
		buffer.len = buf->GetLength();
		buffer.base = (char*)buf->GetData();
		uv_buf_init(buffer.base, buffer.len);
		//NetSocketUV::GenerateRTSPURL(buf);
		/*uv_thread_t proxyThread;
		uv_thread_create(&proxyThread, NetSocketUV::GenerateRTSPURL, nullptr);
		uv_thread_join(&proxyThread);*/
	}
	OnCloseSocket((uv_handle_t*)GetPtrTCP(sock));
}

void NetSocketUV::SendUDP(NET_BUFFER_INDEX *buf)
{
}

void onOpenFile(uv_fs_t* req)
{
	int offset = offsetof(NetBufferUV, sender_object);
	NetBufferUV* buf = (NetBufferUV*)(((char*)req) - offset);
	assert(buf->GetData() != nullptr);
	int result = req->result;
	uv_fs_t read_req;

	uv_buf_t buffer;
	buffer.len = buf->GetLength();
	buffer.base = (char*)buf->GetData();
	uv_buf_init(buffer.base, buffer.len);

	if (result < 0)
	{
		std::cout << "cannot open file" << uv_strerror(result) << std::endl;
	}
#ifdef UNIX
	int w = uv_fs_write(req->loop, &read_req, req->result, req->bufs, 1, 0, OnWriteFile);
	assert(w == 0);
	uv_fs_close(req->loop, req, req->file, nullptr);
#else
	uv_fs_req_cleanup(req);
	int w = uv_fs_write(buf->GetPtrWrite()->handle->loop, &read_req, result, &buffer, 50, offset, OnWriteFile);
	assert(w == 0);
	
#endif // UNIX

	/*for (int i = 0; i < offset; ++i)				  
		outFile.write((const char*)buf->GetData(), i);*/
}

void NetSocketUV::ReceiveTCP()
{
	CStreamFile* f = new CStreamFile;
	NetBuffer* recv_buffer = net->GetRecvBuffer();
	int received_bytes = recv_buffer->GetLength();
	recv_buffer->Add(received_bytes, (void*)recv_buffer->GetData());

	if (f->Open("test_h.264", STREAM_WRITE && STREAM_READ))
	{
		std::cout << "start recording stream in file" << std::endl;
		unsigned int bytes = f->Write(recv_buffer->GetData(), recv_buffer->GetLength());
		std::cout << "bytes recorded: " << bytes << std::endl;
		
		f->Close();
}
	else
	{
		std::cout << "cannot open file to record stream!" << std::endl;
	}

	NET_BUFFER_INDEX* index = net->PrepareMessage(10, received_bytes, recv_buffer->GetData());
	assert(index);
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
		assert(buf->base == (char*)recv_buff->GetData());
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

	//recv_buffer->Add(buf->len, buf->base);
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

void onCloseFile(uv_fs_t* req)
{
	uv_fs_req_cleanup(req);
}

void OnWriteFile(uv_fs_t* req)
{
#ifdef WIN32
	uv_fs_close(req->loop, req, req->file.fd, onCloseFile);
#else
	uv_fs_close(req->loop, req, req->file, onCloseFile);
#endif
	//NetSocketUV::GenerateRTSPURL(nullptr);	
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

void OnListining(void* tcp)
{
	int l = uv_listen((uv_stream_t*)tcp, 1024, OnAccept);
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
	net_sock->Accept((uv_handle_t*)stream);
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
	//outFile.close();
	NetSocket::Destroy();
}



uv_poll_t* GetPtrPoll(void* ptr)
{
	return (uv_poll_t*)(((char*)ptr) + sizeof(void*));
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

