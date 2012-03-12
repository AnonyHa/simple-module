#ifndef __SOCKET_MANAGER_H__
#define __SOCKET_MANAGER_H__


#include "server_socket.h"
#include "client_socket.h"
#include "peer_point.h"
#include "struct_sock.h"
#include <map>
#include <vector>


class SocketManager {
	private:
		map< int, vector<int> > ServerList;
		vector<int> ClientList;
		map<int, StructSock *> PeerList;
	public:
		clsServerSocket* CreateServerSocket(int Port);		
		clsClientSocket* CreateClientSocket(string Ip, int Port);
		bool AddServerVfdList(int ServerVfd, int ClientVfd);
		bool AddClientVfdList(int ClientVfd);
		bool AddServerPeerVfdList(int PeerVfd, clsPeerPoint* PeerObj, clsServerSocket* ServerObj);
		bool AddClientPeerVfdList(int PeerVfd, clsPeerPoint* PeerObj, clsClientSocket* ClientObj);
		void ShowInfo();
};

#endif
