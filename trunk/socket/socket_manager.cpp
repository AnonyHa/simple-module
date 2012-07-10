#include "socket_exception.h"
#include "socket_manager.h"

#include <iostream>
#include <string>

clsServerSocket* SocketManager::CreateServerSocket(int Port, PacketInterface* PacketClass)
{
	clsServerSocket* ServerSock = new clsServerSocket(Port, PacketClass);
	try{
		ServerSock->Start();
	}
	catch(SocketError& e)
	{
		cout << e.GetMsg()<<endl;
		delete ServerSock;
		return NULL;
	}

	cout << "Create Server At Port="<<Port<<endl;
	return ServerSock;
}

clsClientSocket* SocketManager::CreateClientSocket(string Ip, int Port, PacketInterface* PacketClass)
{
	clsClientSocket* ClientSock = new clsClientSocket(Ip, Port, PacketClass);
	try{
		ClientSock->Connect();
	}
	catch(SocketError& e)
	{
		cout << e.GetMsg() <<endl;
		delete ClientSock;
		return NULL;
	}

	cout << "Create Client conn Ok,Ip="<<Ip<<",Port="<<Port<<endl;
	return ClientSock;
}

bool SocketManager::AddPeerServerMap(int PeerVfd, clsServerSocket* ServerObj)
{
	if (PeerVfd2ServerObj.count(PeerVfd)) return false;
	PeerVfd2ServerObj[PeerVfd] = ServerObj;	
	return true;
}

bool SocketManager::AddPeerClientMap(int PeerVfd, clsClientSocket* ClientObj)
{
	if (PeerVfd2ClientObj.count(PeerVfd)) return false;
	PeerVfd2ClientObj[PeerVfd] = ClientObj;	
	return true;
}

bool SocketManager::AddServerObj(int ServerVfd, clsServerSocket* ServerObj)
{
		if (ServerObjList.count(ServerVfd)) return false;
		ServerObjList[ServerVfd] = ServerObj;
		return true;
}

bool SocketManager::AddClientObj(int ClientVfd, clsClientSocket* ClientObj)
{
		if (ClientObjList.count(ClientVfd)) return false;
		ClientObjList[ClientVfd] = ClientObj;
		return true;
}

bool SocketManager::AddPeerObj(int PeerVfd, clsPeerPoint* PeerObj)
{
		if (PeerObjList.count(PeerVfd)) return false;
		PeerObjList[PeerVfd] = PeerObj;
		return true;
}


void SocketManager::ShowInfo()
{
	cout << "Server Vfd Info:"<<endl;
	map< int, clsServerSocket*>::iterator IterServerObj = ServerObjList.begin();
	map< int, clsServerSocket*>::iterator IterPeer2Ser = PeerVfd2ServerObj.begin();

	while(IterServerObj!=ServerObjList.end())
	{
		cout << IterServerObj->first<<"={";
		clsServerSocket* ServerSock = IterServerObj->second;
		IterPeer2Ser = PeerVfd2ServerObj.begin();
		while(IterPeer2Ser!=PeerVfd2ServerObj.end())
		{
			if (IterPeer2Ser->second == ServerSock)
				cout << IterPeer2Ser->first<<",";
			IterPeer2Ser++;
		}
		IterServerObj++;
		cout << "},"<<endl;
	}

	cout << "Client Vfd Info:"<<endl;
	cout << "{";
	map< int, clsClientSocket*>::iterator IterClientObj = ClientObjList.begin();
	while(IterClientObj!=ClientObjList.end())
	{
		cout << IterClientObj->first<<",";
		IterClientObj++;
	}
	cout << "}"<<endl;

	cout << "Peer Vfd Info:"<<endl;
	cout << "{";
	map< int,clsPeerPoint*>::iterator IterPeerObj = PeerObjList.begin();
	while(IterPeerObj!=PeerObjList.end())
	{
		cout << IterPeerObj->first<<",";
		IterPeerObj++;
	}
	cout << "}"<<endl;
}

bool SocketManager::PeerVfdOnClose(int PeerVfd)
{
	clsPeerPoint* p = NULL;
	GET_MAP_VALUE(PeerObjList, PeerVfd, p);
	if (!p) return false;

	if(p->GetVfdType() == SERVER_TYPE)
	{
		clsServerSocket* ServerSock = NULL;
		GET_MAP_VALUE(PeerVfd2ServerObj, PeerVfd, ServerSock);
		if (!ServerSock) return false;

		if (ServerSock->GetPacketInterface())	
		{
			ServerSock->GetPacketInterface()->PacketOnError(PeerVfd);
		}
	}
	else if(p->GetVfdType() == CLIENT_TYPE)
	{
		clsClientSocket* ClientSock = NULL;
		GET_MAP_VALUE(PeerVfd2ClientObj, PeerVfd, ClientSock);
		if (!ClientSock) return false;

		if (ClientSock->GetPacketInterface())
		{
			ClientSock->GetPacketInterface()->PacketOnError(PeerVfd);
		}
	}


	return DelPeerVfd(p->GetVfd(), p->GetVfdType());
}

bool SocketManager::DelPeerVfd(int PeerVfd, int DelType)
{
	int PeerVfdExsit = 0;
	int PeerObjExsit = PeerObjList.count(PeerVfd);	
	bool OpClient = true;
	// 把能够找到的相应内容都删除掉，以免影响其它的对应
	if (PeerObjExsit)
	{
		clsPeerPoint* PeerObj = PeerObjList[PeerVfd];
		PeerObjList.erase(PeerVfd);
		delete PeerObj;
	}

	if(DelType == SERVER_TYPE)
	{
		if (PeerVfd2ServerObj.count(PeerVfd))
		{
			PeerVfd2ServerObj.erase(PeerVfd);
			PeerVfdExsit = 1;
		}
	}
	else
	{
		if (PeerVfd2ClientObj.count(PeerVfd))
		{
			PeerVfd2ClientObj.erase(PeerVfd);
			PeerVfdExsit = 1;
			OpClient = DelClientVfd(PeerVfd);
		}
	}

	bool OpPeer = (PeerVfdExsit && PeerObjExsit)?true:false ;
	return OpPeer && OpPeer;
}

//删除ServerVfd的时候，还需要断开所有的Vfd连接以及相应的资源
bool SocketManager::DelServerVfd(int ServerVfd)
{
	//对PeerVfd2ServerObj遍历，如果有ServerVfd的内容，则删除相应的PeerObj
	map<int, clsServerSocket*>::iterator Iter = PeerVfd2ServerObj.begin();
	int PeerVfd;
	while(Iter!=PeerVfd2ServerObj.end())
	{
		if ((Iter->second)->GetVfd() == ServerVfd) 
		{
			PeerVfd = Iter->first;
			close(PeerVfd);
			DelPeerVfd(PeerVfd, SERVER_TYPE);
			PeerVfd2ServerObj.erase(Iter++);
		}
		else
			Iter++;
	}

	if (ServerObjList.count(ServerVfd))
	{
		clsServerSocket* ServerObj = ServerObjList[ServerVfd];
		delete ServerObj;
		ServerObjList.erase(ServerVfd);
	}

	return true;
}

bool SocketManager::DelClientVfd(int ClientVfd)
{
	if (ClientObjList.count(ClientVfd))
	{
		clsClientSocket* ClientObj = ClientObjList[ClientVfd];
		delete ClientObj;
		ClientObjList.erase(ClientVfd);
		return true;
	}

	return false;
}

bool SocketManager::PeerVfdOnRead(int PeerVfd, char* Buf, int BufLen)
{
	if (! PeerObjList.count(PeerVfd)) return false;
	clsPeerPoint * p = PeerObjList[PeerVfd];	

	if(p->GetVfdType() == SERVER_TYPE)
	{
		clsServerSocket* ServerSock = NULL;
		GET_MAP_VALUE(PeerVfd2ServerObj, PeerVfd, ServerSock);
		if (!ServerSock) return false;

		if (ServerSock->GetPacketInterface())	
		{
			return ServerSock->GetPacketInterface()->PacketOnRead(PeerVfd, Buf, BufLen);
		}
	}
	else if(p->GetVfdType() == CLIENT_TYPE)
	{
		clsClientSocket* ClientSock = NULL;
		GET_MAP_VALUE(PeerVfd2ClientObj, PeerVfd, ClientSock);
		if (!ClientSock) return false;

		if (ClientSock->GetPacketInterface())
		{
			return ClientSock->GetPacketInterface()->PacketOnRead(PeerVfd, Buf, BufLen);
		}
	}
}

bool SocketManager::PeerVfdOnWrite(int ToVfd, char* Buf, int BufLen)
{
	clsPeerPoint * PeerObj = NULL;
	GET_MAP_VALUE(PeerObjList, ToVfd, PeerObj);
	if (!PeerObj) return 0;

	return PeerObj->WriteData(Buf, BufLen);
}

bool SocketManager::PeerVfdOnConnect(int Vfd)
{
	clsPeerPoint* p = NULL;
	GET_MAP_VALUE(PeerObjList, Vfd, p);
	if (!p) return false;

	if(p->GetVfdType() == SERVER_TYPE)
	{
		clsServerSocket* ServerSock = NULL;
		GET_MAP_VALUE(PeerVfd2ServerObj, Vfd, ServerSock);
		if(!ServerSock) return false;

		if (ServerSock->GetPacketInterface())	
		{
			return ServerSock->GetPacketInterface()->PacketOnConnect(Vfd);
		}
	}
	else if(p->GetVfdType() == CLIENT_TYPE)
	{
		clsClientSocket* ClientSock = NULL;
		GET_MAP_VALUE(PeerVfd2ClientObj, Vfd, ClientSock);
		if(!ClientSock) return false;
		if (ClientSock->GetPacketInterface())
		{
			return ClientSock->GetPacketInterface()->PacketOnConnect(Vfd);
		}
	}
}
