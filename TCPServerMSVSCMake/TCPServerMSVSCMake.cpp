// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"

int main() {
	sockaddr* addr;
	servloop = uv_default_loop();
	server = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	Server serv;
	addr = (sockaddr*)malloc(sizeof(sockaddr));
	serv.connect(addr, server);
	std::cout << "main" << std::endl;
	system("pause");
	return 0;
}
