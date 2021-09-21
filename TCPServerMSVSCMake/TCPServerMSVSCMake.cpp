// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"
#include <thread>

int main() {

	memset(addr, 0, sizeof(sockaddr));
	
	Server* serv = (Server*)malloc(sizeof(Server*));
	serv->connect(addr, server);
	std::cout << "Main:" << std::endl;
	std::system("pause");
	return 0;
}
