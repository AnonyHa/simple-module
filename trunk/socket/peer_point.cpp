#include "peer_point.h"
#include "socket_exception.h"
#include "socket_manager.h"

#include <iostream>

using namespace std;
extern SocketManager* Manager;

void PeerOnRead(struct bufferevent * buf_ev, void * arg)
{
	clsPeerPoint* p = static_cast<clsPeerPoint *>(arg);
	p->OnRead(buf_ev, arg);
}

void PeerOnError(struct bufferevent * buf_ev, short error_no, void * arg)
{
	clsPeerPoint* p = static_cast<clsPeerPoint *>(arg);
	p->OnError(buf_ev, error_no, arg);
}

clsPeerPoint::clsPeerPoint(int Vfd)
{
	_Vfd = Vfd;
	_BuffEv = bufferevent_new(_Vfd, PeerOnRead, NULL, PeerOnError, this);
	bufferevent_enable(_BuffEv, EV_READ);
	_Status = 1;
}

clsPeerPoint::~clsPeerPoint()
{
}

void clsPeerPoint::OnRead(struct bufferevent* buf_ev, void* arg)
{
	if(_Status == 0) throw SocketError(_Vfd, 1, "Try To Read Cleared vfd");

	int ReadLen = 0;
	char buf[RECV_BLOCK_SIZE];	

	while(1) {
		ReadLen = bufferevent_read(buf_ev, buf, sizeof(buf));
		if (ReadLen == 0) return;
		
		for(int i=0;i<ReadLen;i++)
			cout<<buf[i];
	}

}

bool clsPeerPoint::OnWrite()
{
	return true;
}

void clsPeerPoint::OnError(struct bufferevent* buf_ev, short error_no, void* arg)
{
	if (_Status == 0) return;
	_Status = 0;
	Manager->DelPeerPointVfd(_Vfd);
	close(_Vfd);
	Manager->ShowInfo();
}
