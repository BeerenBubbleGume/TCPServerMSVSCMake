#include "Net.h"

Net::Net()
{
#ifdef WIN32
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
#else
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
#endif // WIN32

}

Net::~Net()
{
}
#ifdef WIN32
void Net::closesock(void* sock)
{
	closesocket((SOCKET)sock);
	WSACleanup();
}
#else 
void Net::closesock(void* socket)
{
	/*
	free(DataBuff);
	close((int*)socket);
	*/
}
#endif // WIN32

