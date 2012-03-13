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

	ClientList.push_back(ClientSock->GetVfd());
	cout << "Create Client conn Ok,Ip="<<Ip<<",Port="<<Port<<endl;
	return ClientSock;
}

bool SocketManager::AddServerVfdList(int ServerVfd, int ClientVfd)
{
	if (!ServerList.count(ServerVfd)) { vector<int > tmp;ServerList[ServerVfd]=tmp;};	
	ServerList[ServerVfd].push_back(ClientVfd);
	return true;
}

bool SocketManager::AddClientVfdList(int ClientVfd)
{
	ClientList.push_back(ClientVfd);
}

bool SocketManager::AddServerPeerVfdList(int PeerVfd, clsPeerPoint* PeerObj, clsServerSocket* ServerObj)
{
	cout << "Just a Server Test;" <<endl;
	if (!PeerList.count(PeerVfd)) 
	{
		cout <<"Insert OK!"<<endl;
		StructSock * tmp = new StructSock();
		tmp->InitServerStruct(PeerObj, ServerObj);
		PeerList[PeerVfd] = tmp;
		cout << PeerList.size() <<endl;
		return true;
	}
	else
	{
		cerr << "PeerVfd="<<PeerVfd<<"has exsit!"<<endl;
		return false;
	}
}

bool SocketManager::AddClientPeerVfdList(int PeerVfd, clsPeerPoint* PeerObj, clsClientSocket* ClientObj)
{
	if (!PeerList.count(PeerVfd)) 
	{
		StructSock * tmp = new StructSock();
		tmp->InitClientStruct(PeerObj, ClientObj);
		PeerList[PeerVfd] = tmp;
		return true;
	}
	else
	{
		cerr << "PeerVfd="<<PeerVfd<<"has exsit!"<<endl;
		return false;
	}
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

bool SocketManager::DelPeerPointVfd(int PeerVfd)
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

bool SocketManager::DelServerPeerVfd(int ServerVfd, int PeerVfd)
{
	if (!ServerList.count(ServerVfd)) return false;
	vector<int>::iterator iter = ServerList[ServerVfd].begin();
	while(iter!=ServerList[ServerVfd].end())
	{
		if (*iter == PeerVfd) 
		{
			ServerList[ServerVfd].erase(iter);
			break;
		}	
		iter++;
	}
	return true;
}

bool SocketManager::DelClientVfd(int ClientVfd)
{
	vector<int>::iterator iter = ClientList.begin();	
	while(iter!=ClientList.end())
	{
		if (*iter == ClientVfd)
		{
			ClientList.erase(iter);
			break;
		}
		iter++;
	}
	return true;
}

bool SocketManager::PeerVfdOnRead(int PeerVfd, char* Buf, int BufLen)
{
	if (!PeerList[PeerVfd]) return false;
	StructSock * p = PeerList[PeerVfd];	
	if(p->GetVfdType() == 1)
	{
		clsServerSocket* ServerSock = p->GetServerSocket();
		if (ServerSock->GetPacketInterface())	
		{
			return ServerSock->GetPacketInterface()->PacketOnRead(PeerVfd, Buf, BufLen);
		}
	}
	else if(p->GetVfdType() == 2)
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
