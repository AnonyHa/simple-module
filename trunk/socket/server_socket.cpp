#include "socket_exception.h"
#include "server_socket.h"
#include "peer_point.h"
#include "event.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>

using namespace std;

bool SetNoNBlock(int fd)
{
	int flags;
	flags = fcntl(fd,F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)	return false;
	return true;
}

void NewConnect(int listenfd, short event, void * arg) {
	//这里要作映射关系存储	
	struct sockaddr_in client_addr;
	socklen_t len_addr = sizeof(client_addr);
	
	int client_fd;
	client_fd = accept(listenfd, (struct sockaddr *)&client_addr, &len_addr);
	if (client_fd < 0)
	{
		throw SocketError(listenfd, 7, "Accept Failed,Can not Create Client Vfd");
		return;
	}
	if (!SetNoNBlock(client_fd))
	{
		//如果设置文件描述符失败，则直接关掉相应的vfd
		close(client_fd);
		throw SocketError(client_fd, 4, "Set Non Block Failed");
		return;
	}	

	clsPeerPoint* PeerObj = new clsPeerPoint(client_fd);
	cout << "Get A New Connection,Vfd=" << client_fd << endl;
}

void clsServerSocket::Start()
{
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0 ){
		throw SocketError(-1, 1, string("Create Server Vfd Failed"));
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
	serveraddr.sin_port = htons(_Port);

	int optval = 1;
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
		close(socketfd);
		throw SocketError(socketfd, 2, string("Set ReUseAddr Failed"));
	}

	if(bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
		close(socketfd);
		throw SocketError(socketfd, 3, string("Bind Failed"));
	}

	if(listen(socketfd, _MaxNumber) != 0){
		close(socketfd);
		throw SocketError(socketfd, 4, string("Listen Failed"));
	}

	int iFlags = fcntl(socketfd, F_GETFL, 0);
	if (iFlags == -1 || fcntl(socketfd, F_SETFL, iFlags | O_NONBLOCK)) {
		close(socketfd);
		throw SocketError(socketfd, 5, string("Set Non Block Failed"));
	}
	
	_Vfd = socketfd;
	struct event* ev_listen = (struct event*)malloc(sizeof(struct event));
	event_set(ev_listen, _Vfd, EV_READ|EV_PERSIST, NewConnect, this);
	event_add(ev_listen, NULL);
}
