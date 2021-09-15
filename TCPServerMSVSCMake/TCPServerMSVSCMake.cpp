// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"


using namespace std;

uv_loop_t* loop = uv_default_loop();
uv_tcp_t serv;
uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));

int main() {

	errno = 0;
	status = errno;
	uv_tcp_init(loop, &serv);

	struct sockaddr_in recive_addr;

	

	uv_ip4_addr("0.0.0.0", 8000, &recive_addr);

	uv_tcp_bind(&serv, (const struct sockaddr*)&recive_addr, 0);

	int ret = uv_listen((uv_stream_t*)&serv, 128, NULL);
	if (ret) {
		fprintf(stderr, "Listean fail: \n", uv_strerror(status));
		return 1;
	}
	on_accept((uv_stream_t*)&serv, status);
	return uv_run(loop, UV_RUN_DEFAULT);
}

void handlingLoop()
{
	
}

void on_accept(uv_stream_t* handler, errno_t status)
{
	if (status < 0) {
		fprintf(stderr, "New connection fail: \n", uv_strerror(status));
		return;
	}

	uv_tcp_init(loop, client);
	if (uv_accept(handler, (uv_stream_t*)client) == 0) {
		uv_read_start((uv_stream_t*)client, alloc_buffer, read_buffer);
	}
	
	uv_close((uv_handle_t*)&handler, on_close_cb);
}

void on_close(uv_tcp_t* handle, int status)
{
	free(handle);
	printf("disconected.\n");
}
