// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"

int main() {

	memset(addr, 0, sizeof(sockaddr));

	Server* serv = (Server*)malloc(sizeof(Server*));
	serv->connect(addr, server);
	std::cout << "Main:" << std::endl;

	return 0;
}
