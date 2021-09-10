// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"


using namespace std;



Server::Server(uint16_t port, uv_loop_t* loop) : port(port), loop(loop){
	loop = uv_default_loop();
	uv_tcp_init(loop, &serv);
	
	int address = uv_ip4_addr("0.0.0.0", port, addr);
	uv_tcp_bind(&serv, (const struct sockaddr*)&address, 0);
	ret = uv_listen((uv_stream_t*)&serv, 128, NULL);
    
}

Server::~Server()
{
	
}

void Server::handlingLoop()
{

}

void Server::on_accept(uv_tcp_t* handler) {
	assert(handler == &serv);
	printf("on conection\n");

	uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	int r = uv_accept((uv_stream_t*)&serv, (uv_stream_t*)&client);
	//uv_fs_t* fs;

	if (r) {
		//int err = uv_fs_get_system_error(fs);
		//fprintf(stderr, "error accept: %s\n", uv_strerror(err));
		return;
	}
}

void Server::on_open(uv_tcp_t* handle, uv_os_sock_t* socket)
{
}

void Server::on_close(uv_tcp_t* handle)
{
	uv_tcp_close_reset(handle, NULL);
}

void Server::on_connection(uv_stream_t* server, int status)
{

}


int main(uint16_t hostm, uv_loop_t* loop){
	
	uv_loop_init(loop);

	loop = uv_default_loop();
	
	uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}