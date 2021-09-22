// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"

int main() {

	ip = {"127.0.0.1"};
	Server* serv = new Server;
	if ((serv->connect(addr, ip)) == 0)
	{
		std::cout << "Main:" << std::endl;
	}

	return 0;
}
