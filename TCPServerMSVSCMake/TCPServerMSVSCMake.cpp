// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"


using namespace std;

Server::Server(uint16_t port) : port(port){
	
	uv_tcp_init(loop, &serv);

	uv_ip4_addr("127.0.0.1", port, &addr);
	uv_tcp_bind(&serv, (const struct sockaddr*)&addr, 0);
	ret = uv_listen((uv_stream_t*)&serv, NULL, NULL);
    
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

int main(uint16_t host, uv_loop_t* loop) {

	uv_loop_init(loop);
	Server(8000);

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}