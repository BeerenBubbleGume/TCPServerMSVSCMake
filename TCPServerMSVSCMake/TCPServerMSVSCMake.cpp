// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"

int main() {

	servloop = uv_default_loop();
	server = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	std::system("pause");
	return 0;
}
