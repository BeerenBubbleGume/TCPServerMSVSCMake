// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"


using namespace std;

uv_loop_t* loop = uv_default_loop();
uv_tcp_t serv;

int main() {


	uv_tcp_init(loop, &serv);
	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", 8000, &addr);
	uv_tcp_bind(&serv, (const struct sockaddr*)&addr, 0);

	int ret = uv_listen((uv_stream_t*)&serv, 128, NULL);

	uv_run(loop, UV_RUN_DEFAULT);
}

void handlingLoop()
{

}

void on_accept(uv_tcp_t* handler)
{

}

void on_close(uv_tcp_t* handler)
{
	
}
