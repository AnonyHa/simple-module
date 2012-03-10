#ifndef __SOCKET_ERROR_H__
#define __SOCKET_ERROR_H__

#include <string>

using namespace std;

class SocketError {
	private:
		string _ErrMsg;
		int _ErrNo;
		int _Vfd;
	public:
		SocketError(int Vfd,int ErrNo,string ErrMsg)
		{
			_Vfd = Vfd;
			_ErrNo = ErrNo;
			_ErrMsg = ErrMsg;
		};

		int GetErrNo() {return _ErrNo;};
		string GetErrMsg() {return _ErrMsg;};
		string GetMsg() {return _ErrMsg;};
		int GetVfd() {return _Vfd;};
		~SocketError() {};
};

#endif
