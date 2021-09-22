#include "Net.h"

Net::Net()
{
}

Net::~Net()
{

}
#ifdef WIN32
void Net::closesock(SOCKET sock)
{
	closesocket(sock);
	free(buffer);
	WSACleanup();
}
#else 
void Net::closesock(int socket)
{
	free(buffer);
	close(socket);
}
#endif // WIN32


Net* Net::Recive()
{
	if (bind(tcp_socket, (struct sockaddr*)&net_addr, sizeof(net_addr)) < 0) {
		perror("bind");
		exit(1);
	}

	while (true)
	{
		tcp_socket = accept(listner, nullptr, nullptr);
		if (tcp_socket < 0)
		{
			perror("Reciving fail!\n");
			exit(2);
		}
		while (true)
		{
			bytes_read = recv(tcp_socket, buffer, 4096, 0);
		}
	}
}

