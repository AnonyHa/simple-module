#include "client_socket.h"
#include "socket_exception.h"
#include "peer_point.h"

#include "event.h"

#include <iostream>
#include <string>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

bool SetNoNBlock(int fd)
{
	int flags;
	flags = fcntl(fd,F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)	return false;
	return true;
}


void clsClient::Connect()
{
	int ClientFd = socket(AF_INET, SOCK_STREAM, 0);
	if(ClientFd < 0) {throw SocketError(-1, 2, GetAimInfo() + "Create File Describtion error!");};

	struct sockaddr_in ClientAddr;
	memset(&ClientAddr, 0, sizeof(ClientAddr));
	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_addr.s_addr = inet_addr(GetIp().c_str());
	ClientAddr.sin_port = htons(GetPort());
	cout << "try to Connect Ip:" << GetIp() << " Port:"<< GetPort() <<endl;

	if(connect(ClientFd, (struct sockaddr *)&ClientAddr, sizeof(ClientAddr))<0) {
		throw SocketError(ClientFd, 2, GetAimInfo() + " Connect Failed");
	}

	if (!SetNoNBlock(ClientFd))
	{
		close(ClientFd);
		throw SocketError(ClientFd, 3, GetAimInfo() + "Set NonBlock Failed");
	}

	clsPeerPoint* PeerObj = new clsPeerPoint(ClientFd);
}

void clsClient::Close()
{
	close(_Vfd);
}

/************  ²âÊÔÓÃ *****************/
int main(void)
{
	event_init();

	clsClient* Test = new clsClient(string("127.0.0.1"), 6666);
	try{
		Test->Connect();
	}
	catch(SocketError& e)
	{
		cout << "Vfd:" << e.GetVfd() << "\tErrMsg:" << e.GetMsg() << endl;
	}

	while(1)
	{
		event_dispatch();
	}

	return 0;
}
