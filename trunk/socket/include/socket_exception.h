#ifndef __SOCKET_ERROR_H__
#define __SOCKET_ERROR_H__
#include <string>

using namespace std;

#define SERVER_CREATE_ERR 0
#define CLIENT_CREATE_ERR 1
#define CONN_ERR 2
#define READ_DATA_ERR 3
#define WRITE_DATA_ERR 4
#define CLEAR_ERR 5
#define SERVER_CREATE_PEER_ERR 6

#define MAX_ERR_MSG 1024


class SocketError {
	private:
		char* _ErrMsg;
		int _ErrNo;
		int _Vfd;

		int GetVfd() {return _Vfd;};
		int GetErrNo() {return _ErrNo;};
		char* GetErrMsg() {return _ErrMsg;};
	public:
		SocketError(int Vfd,int ErrNo,char* ErrMsg);
		SocketError(int Vfd,int ErrNo,string ErrMsg);
		char* GetMsg();
		~SocketError() {};
};

#endif
