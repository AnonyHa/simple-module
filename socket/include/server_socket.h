#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

class clsServerSocket{
	private:
		int _Vfd;
		int _Port;
		int _MaxNumber;
	public:
		clsServerSocket(int Port):_MaxNumber(1024) {_Port=Port;};
		void Start();
		int GetServerVfd() {return _Vfd;};
		int GetPort() {return _Port;};
};

#endif
