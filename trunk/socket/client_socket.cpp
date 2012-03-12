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

void clsClientSocket::Connect()
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


	int iFlags = fcntl(ClientFd, F_GETFL, 0);
	if (iFlags == -1 || fcntl(ClientFd, F_SETFL, iFlags | O_NONBLOCK)) {
		close(ClientFd);
		throw SocketError(ClientFd, 3, GetAimInfo() + "Set NonBlock Failed");
	}

	clsPeerPoint* PeerObj = new clsPeerPoint(ClientFd);
}

void clsClientSocket::Close()
{
	close(_Vfd);
}
