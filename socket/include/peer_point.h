#ifndef __PEER_POINT_H__
#define __PEER_POINT_H__

#include "event.h"
#define RECV_BLOCK_SIZE 1024

class clsPeerPoint{
	private:
		int _Vfd;
		int _VfdType;
		bufferevent * _BuffEv;
		int _Status;
	public:
		clsPeerPoint(int Vfd, int VfdType);
		~clsPeerPoint();
		void OnRead(struct bufferevent * buf_ev, void * arg);
		void OnError(struct bufferevent * buf_ev, short error_no, void * arg);
		void OnConnect(int Vfd);
		bool WriteData(char* Buf, int BufLen);
		int GetVfd() {return _Vfd;};
		int GetVfdType() {return _VfdType;};
};

#endif
