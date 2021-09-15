// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"


using namespace std;

int main() {
	Server serv;
	struct sockaddr_in addr;
	serv.connect((sockaddr*)&addr);
	return 0;
}
/*
void handlingLoop()
{
	
}

void on_accept(uv_stream_t* handler, errno_t status)
{
	if (status < 0) {
		fprintf(stderr, "New connection fail: \n", uv_strerror(status));
		return;
	}

	uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
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
*/