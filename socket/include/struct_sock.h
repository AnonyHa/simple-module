#ifndef __STRUCT_SOCK_H__
#define __STRUCT_SOCK_H__

#include "server_socket.h"
#include "client_socket.h"
#include "peer_point.h"

class StructSock{
	private:
		int _SocketVfdType; //1表示服务端生成的Vfd,2表示客户端直连生成的Vfd
		clsPeerPoint * _PeerObj;
		clsServerSocket * _ServerSock;
		clsClientSocket* _ClientSock;
	public:
		bool InitServerStruct(clsPeerPoint* PeerObj, clsServerSocket* ServerSock)
		{
			_PeerObj = PeerObj;
			_ServerSock = ServerSock;
			_SocketVfdType = 1;
			return true;
		};
		bool InitClientStruct(clsPeerPoint* PeerObj, clsClientSocket* ClientSock)
		{
			_PeerObj = PeerObj;
			_ClientSock = ClientSock;
			_SocketVfdType = 2;
			return true;
		};
		int GetVfdType() {return _SocketVfdType;};
		clsServerSocket* GetServerSocket(){return _ServerSock;};
		clsClientSocket* GetClientSocket(){return _ClientSock;};
};

#endif
