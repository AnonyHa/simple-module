#include <iostream>
#include <string>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "event.h"

using namespace std;

void NewConnect(int listenfd, short event, void * arg) {
	struct sockaddr_in client_addr;
	socklen_t len_addr = sizeof(client_addr);

	int client_fd;
	client_fd = accept(listenfd, (struct sockaddr *)&client_addr, &len_addr);
	if (client_fd < 0)
		cerr<< "accept error!"<<endl;

	int flags;
	flags = fcntl(client_fd, F_GETFL);
	flags |= O_NONBLOCK;
	if(fcntl(client_fd, F_SETFL, flags)<0)
	{
		close(client_fd);
		return;
	}

	cout << "Get New Conn" <<endl;
	char buf[] = "Send Msg:abcd\n";
	write(client_fd, buf, sizeof(buf));
	close(client_fd);
}


bool InitServer(const int Port, int & ServerFd) {
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0 ){
		return false;
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
	serveraddr.sin_port = htons(Port);

	int optval = 1;
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
		return false;
	}

	if(bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
		return false;
	}

	if(listen(socketfd, 10) != 0){
		close(socketfd);
		return false;
	}

	int iFlags = fcntl(socketfd, F_GETFL, 0);
	if (iFlags == -1 || fcntl(socketfd, F_SETFL, iFlags | O_NONBLOCK)) {
		return false;
	}
	
	ServerFd = socketfd;
	return true;
}

int main(void)
{
	int ServerVfd;
	if(!InitServer(6666, ServerVfd)) cerr<<"Start Server Error!"<<endl;
	cout << "Start Server At Port:"<<6666<<endl;

	event_init();
	struct event ev_listen;
	event_set(&ev_listen, ServerVfd, EV_READ|EV_PERSIST, NewConnect, NULL);
	event_add(&ev_listen, NULL);

	event_dispatch();
}
