#include "socket_manager.h"
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
#include <stdlib.h>

extern SocketManager* Manager;

using namespace std;

void NewConnect(int listenfd, short event, void * arg) {
	//这里要作映射关系存储	
	struct sockaddr_in client_addr;
	socklen_t len_addr = sizeof(client_addr);
	clsServerSocket* ServerSock = (clsServerSocket*)arg;
	
	int client_fd;
	client_fd = accept(listenfd, (struct sockaddr *)&client_addr, &len_addr);
	if (client_fd < 0)
	{
		throw SocketError(listenfd, SERVER_CREATE_PEER_ERR, "Accept Failed,Can not Create Client Vfd");
		return;
	}

	int iFlags = fcntl(client_fd, F_GETFL, 0);
	if (iFlags == -1 || fcntl(client_fd, F_SETFL, iFlags | O_NONBLOCK)) {
		//如果设置文件描述符失败，则直接关掉相应的vfd
		close(client_fd);
		throw SocketError(client_fd, SERVER_CREATE_PEER_ERR, "Set Non Block Failed");
		return;
	}	

	clsPeerPoint* PeerObj = new clsPeerPoint(client_fd, SERVER_TYPE);

	Manager->AddPeerObj(client_fd, PeerObj);
	Manager->AddPeerServerMap(client_fd, ServerSock);

	PeerObj->OnConnect(client_fd);
}

void clsServerSocket::Start()
{
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0 ){
		throw SocketError(socketfd, SERVER_CREATE_ERR, "Create Server Vfd Failed");
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
	serveraddr.sin_port = htons(_Port);

	int optval = 1;
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
		close(socketfd);
		throw SocketError(socketfd, SERVER_CREATE_ERR, "Set ReUseAddr Failed");
	}

	if(bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
		close(socketfd);
		throw SocketError(socketfd, SERVER_CREATE_ERR, "Bind Failed");
	}

	if(listen(socketfd, _MaxNumber) != 0){
		close(socketfd);
		throw SocketError(socketfd, SERVER_CREATE_ERR, "Listen Failed");
	}

	int iFlags = fcntl(socketfd, F_GETFL, 0);
	if (iFlags == -1 || fcntl(socketfd, F_SETFL, iFlags | O_NONBLOCK)) {
		close(socketfd);
		throw SocketError(socketfd, SERVER_CREATE_ERR, "Set Non Block Failed");
	}
	
	_Vfd = socketfd;
	struct event* ev_listen = (struct event*)malloc(sizeof(struct event));
	event_set(ev_listen, _Vfd, EV_READ|EV_PERSIST, NewConnect, this);
	event_add(ev_listen, NULL);

	Manager->AddServerObj(_Vfd, this);
}
