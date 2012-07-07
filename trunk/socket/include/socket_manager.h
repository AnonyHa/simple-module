#ifndef __SOCKET_MANAGER_H__
#define __SOCKET_MANAGER_H__


#include "server_socket.h"
#include "client_socket.h"
#include "peer_point.h"
#include "struct_sock.h"
#include "packet_interface.h"

#include <map>
#include <vector>

#define SERVER_TYPE 1
#define CLIENT_TYPE 2

#define GET_MAP_VALUE(Map,Key,RetVal) {if (Map.count(Key)) RetVal=Map[Key];}

class SocketManager {
	private:
		//三个不同的ObjMap
		map< int, clsServerSocket* > ServerObjList;
		map< int, clsClientSocket* > ClientObjList;
		map< int, clsPeerPoint* > PeerObjList;
		//核心的抽象对象 PeerVfd与ServerVfd,以及ClientVfd的映射
		map< int, clsServerSocket*> PeerVfd2ServerObj;
		map< int, clsClientSocket*> PeerVfd2ClientObj;


		bool DelClientVfd(int ClientVfd);
		bool DelServerVfd(int ServerVfd);
		bool DelPeerVfd(int PeerVfd, int DelType);
	public:
		clsServerSocket* CreateServerSocket(int Port, PacketInterface* PacketClass = 0);		
		clsClientSocket* CreateClientSocket(string Ip, int Port, PacketInterface* PacketClass = 0);

		bool AddPeerServerMap(int PeerVfd, clsServerSocket* ServerObj);
		bool AddPeerClientMap(int PeerVfd, clsClientSocket* ClientObj);
		bool AddServerObj(int ServerVfd, clsServerSocket* ServerObj);
		bool AddClientObj(int ClientVfd, clsClientSocket* ClientObj);
		bool AddPeerObj(int PeerVfd, clsPeerPoint* PeerObj);

		void ShowInfo();
		bool PeerVfdOnClose(int PeerVfd);
		bool PeerVfdOnRead(int PeerVfd, char* Buf, int BufLen);
		bool PeerVfdOnWrite(int ToVfd, char* Buf, int BufLen);
		bool PeerVfdOnConnect(int Vfd);
};

#endif
