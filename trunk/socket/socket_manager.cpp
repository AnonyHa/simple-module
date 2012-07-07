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
		cout << "Create Server Error,Port="<<Port<<" "<<e.GetMsg()<<endl;
		delete ServerSock;
		return NULL;
	}

	int ServerVfd = ServerSock->GetVfd();
	if (!AddServerObj(ServerVfd, ServerSock))
	{
		delete ServerSock;
		throw SocketError(ServerVfd, 4, "ServerVfd has Obj");
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
		cout << "Create Client Error,Ip="<<Ip <<"Port="<<Port<<",ErrMsg="<<e.GetMsg()<<endl;
		delete ClientSock;
		return NULL;
	}

	int ClientVfd = ClientSock->GetVfd();
	if (!AddClientObj(ClientVfd, ClientSock))
	{
		delete ClientSock;
		throw SocketError(ClientVfd, 4, "ClientVfd has Obj");
		return NULL;
	}

	//这里要添加PeerVfd与ClientVfd的映射
	AddPeerClientVfdMap(ClientVfd, ClientVfd);

	cout << "Create Client conn Ok,Ip="<<Ip<<",Port="<<Port<<endl;
	return ClientSock;
}

bool SocketManager::AddPeerServerVfdMap(int PeerVfd, int MapVfd)
{
	if (PeerVfd2ServerVfd.count(PeerVfd)) return false;
	PeerVfd2ServerVfd[PeerVfd] = MapVfd;	
	return true;
}

bool SocketManager:AddPeerClientVfdMap(int PeerVfd, int MapVfd)
{
	if (PeerVfd2ClientVfd.count(PeerVfd)) return false;
	PeerVfd2ClientVfd[PeerVfd] = MapVfd;	
	return true;
}


bool SocketManager:AddServerObj(int ServerVfd, clsServerSocket* ServerObj)
{
	if (ServerObjList.count(ServerVfd)) return false;
	ServerObjList[ServerVfd] = ServerObj;
	return true;
}

bool SocketManager:AddClientObj(int ClientVfd, clsClientSocket* ClientObj)
{
	if (ClientObjList.count(ClientVfd)) return false;
	ClientObjList[ClientVfd] = ClientObj;
	return true;
}

bool SocketManager:AddPeerObj(int PeerVfd, clsPeerPoint* PeerObj)
{
	if (PeerObjList.count(PeerVfd)) return false;
	PeerObjList[PeerVfd] = PeerObj;
	return true;
}


void SocketManager::ShowInfo()
{
	int TmpVfd;
	cout <<"ServerVfd Info:"<<endl;
	map< int, vector<int> >::iterator iter = ServerList.begin();
	map< int, vector<int> >::iterator iter_end = ServerList.end();
	while(iter != iter_end)
	{
		cout << "ServerVfd="<<iter->first << ",PeerVfdList={";
		vector<int>::iterator subiter = (iter->second).begin();
		while(subiter!= (iter->second).end())
		{
			cout << *subiter<<",";
			TmpVfd = *subiter;
			subiter++;
		}
		cout << "}" <<endl;
		iter++;
	}


	cout <<"ClientVfd Info:"<<endl<<"ClientVfd:";
	vector<int>::iterator ClientIter = ClientList.begin();
	while(ClientIter!= ClientList.end())
	{
		cout << *ClientIter;
		ClientIter++;
	}
	cout << endl;
	cout <<"PeerVfd Info:"<<endl<<"PeerVfd:";

	map< int, StructSock * >::iterator PeerIter = PeerList.begin();
	while(PeerIter!=PeerList.end())
	{
		cout << PeerIter->first<<",";
		PeerIter++;
	}
	cout << endl;
}

bool SocketManager::PeerVfdOnClose(int PeerVfd)
{
	if(!PeerList.count(PeerVfd))	
	{
		cerr<< "PeerVfd="<<PeerVfd<<"not exsit!"<<endl;
		return false;
	}

	//先对ServerList和ClientList作操作
	StructSock* p = PeerList[PeerVfd];
	if(p->GetVfdType() == 1)
	{
		int ServerVfd = p->GetServerSocket()->GetServerVfd();
		DelServerPeerVfd(ServerVfd, PeerVfd);
	}
	else if(p->GetVfdType() == 2)
	{
		DelClientVfd(PeerVfd);
	}
	else
	{
		cerr<<"PeerVfd="<<PeerVfd<<",Socket Vfd Type="<<p->GetVfdType()<<endl;
		return false;
	}	

	//针对PeerVfd作操作
	PeerList.erase(PeerVfd);
	delete p;	

	return true;
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
		if (PeerVfd2ServerVfd.count(PeerVfd))
		{
			PeerVfd2ServerVfd.erase(PeerVfd);
			PeerVfdExsit = 1;
		}
	}
	else
	{
		if (PeerVfd2ClientVfd.count(PeerVfd))
		{
			PeerVfd2ClientVfd.erase(PeerVfd);
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
	//对PeerVfd2ServerVfd遍历，如果有ServerVfd的内容，则删除相应的PeerObj
	map<int, int>::iterator Iter = PeerVfd2ServerVfd.begin();
	int PeerVfd;
	while(Iter!=PeerVfd2ServerVfd.end())
	{
		if (Iter->second == ServerVfd) 
		{
			PeerVfd = Iter->first;
			close(PeerVfd);
			DelPeerVfd(PeerVfd, SERVER_TYPE);
			PeerVfd2ServerVfd.erase(Iter++);
		}
		else
			Iter++;
	}

	if (ServerObjList.count(ServerVfd))
	{
		clsServerSocket* ServerObj = ServerObjList[ServerVfd];
		delete ServerObj;
		ServerObjList.erase(SeverVfd);
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
		int ServerVfd = -1; 
		GET_MAP_VALUE(PeerVfd2ServerVfd, PeerVfd, ServerVfd);
		if (ServerVfd == -1) return false;	
		clsServerSocket* ServerSock = NULL;
		GET_MAP_VALUE(ServerObjList, ServerVfd, ServerSock);
		if (!ServerSock) return false;

		if (ServerSock->GetPacketInterface())	
		{
			return ServerSock->GetPacketInterface()->PacketOnRead(PeerVfd, Buf, BufLen);
		}
	}
	else if(p->GetVfdType() == CLIENT_TYPE)
	{
		clsClientSocket* ClientSock = p->GetClientSocket();
		if (ClientSock->GetPacketInterface())
		{
			return ClientSock->GetPacketInterface()->PacketOnRead(PeerVfd, Buf, BufLen);
		}
	}
}

bool SocketManager::PeerVfdOnWrite(int ToVfd, char* Buf, int BufLen)
{
	if(!PeerList[ToVfd]) return false;
	write(ToVfd, Buf, BufLen);
	return true;
}

bool SocketManager::PeerVfdOnConnect(int Vfd)
{
	if(!PeerList[Vfd]) return false;
	StructSock * p = PeerList[Vfd];	
	if(p->GetVfdType() == 1)
	{
		clsServerSocket* ServerSock = p->GetServerSocket();
		if (ServerSock->GetPacketInterface())	
		{
			return ServerSock->GetPacketInterface()->PacketOnConnect(Vfd);
		}
	}
	else if(p->GetVfdType() == 2)
	{
		clsClientSocket* ClientSock = p->GetClientSocket();
		if (ClientSock->GetPacketInterface())
		{
			return ClientSock->GetPacketInterface()->PacketOnConnect(Vfd);
		}
	}
}
