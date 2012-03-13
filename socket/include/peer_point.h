#ifndef __PEER_POINT_H__
#define __PEER_POINT_H__

#include "event.h"
#define RECV_BLOCK_SIZE 1024

class clsPeerPoint{
	private:
		int _Vfd;
		bufferevent * _BuffEv;
		int _Status;
	public:
		clsPeerPoint(int Vfd);
		~clsPeerPoint();
		void OnRead(struct bufferevent * buf_ev, void * arg);
		void OnError(struct bufferevent * buf_ev, short error_no, void * arg);
};

#endif
