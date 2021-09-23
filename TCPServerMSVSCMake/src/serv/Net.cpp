#include "Net.h"

Net::Net()
{
}

Net::~Net()
{

}
#ifdef WIN32
void Net::closesock(void* sock)
{
	closesocket((SOCKET)sock);
	free(DataBuff);
	WSACleanup();
}
#else 
void Net::closesock(void* socket)
{
	free(DataBuff);
	close(socket);
}
#endif // WIN32

