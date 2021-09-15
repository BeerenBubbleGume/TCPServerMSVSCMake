#include "ServerUV.h"

uv_loop_t* loop;

Server::Server()
{
	servloop = uv_default_loop();
	serv = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_loop_init(servloop);
	uv_tcp_init(servloop, serv);
	uv_run(servloop, UV_RUN_DEFAULT);
}

Server::~Server()
{
	free(servloop);
	
}

int Server::connect(sockaddr* addr)
{
	uv_ip4_addr("0.0.0.0", 544, (sockaddr_in*)addr);
	uv_listen((uv_stream_t*)serv, 1024, OnAccept);
	return uv_tcp_connect(req, handle, addr, OnConnection);
}

void Server::setup()
{
	uv_tcp_nodelay(serv, true);
}

void OnConnection(uv_connect_t* req, int status)
{
	if (status == 0) {

	}
	else {

	}
}

void OnAccept(uv_stream_t* server, int status)
{
	if (status < 0)
	{
		// ќшибка установки соединени€
		return;
	}
	// —ервер создает новый сокет, чтобы через него установить соединение с клиентом
	uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	
	uv_tcp_init(loop, client);
	if (uv_accept(server, (uv_stream_t*)client) == 0) // делаетс€ попытка установить соединение
	{
		// —оединение установлено
		uv_read_start((uv_stream_t*)client, OnAllocBuffer, OnReadTCP); // ѕереводим сокет в режим чтени€
	}
	else
	{
		// —оединение установить не удалось
		//uv_close((uv_handle_t*)tcp_socket, OnCloseSocket); // ”дал€ем сокет, который был создан дл€ соединени€
	}
}

void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	*buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
	buf->base = (char*)suggested_size;
}

void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	if (nread < 0) {
		if (nread == UV_EOF) {
			// end of file
			uv_close((uv_handle_t*)stream, OnCloseSocket);
		}
	}
	else if (nread > 0) {

	}

	if (buf->base)
		free(buf->base);
}