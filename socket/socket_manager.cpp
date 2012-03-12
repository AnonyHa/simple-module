#include "socket_exception.h"
#include "socket_manager.h"

#include <iostream>
#include <string>

clsServerSocket* SocketManager::CreateServerSocket(int Port)
{
	clsServerSocket* ServerSock = new clsServerSocket(Port);
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

clsClientSocket* SocketManager::CreateClientSocket(string Ip, int Port)
{
	clsClientSocket* ClientSock = new clsClientSocket(Ip, Port);
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
	cout <<"ServerVfd Info:"<<endl;
	map< int, vector<int> >::iterator iter = ServerList.begin();
	map< int, vector<int> >::iterator iter_end = ServerList.end();
	while(iter != iter_end)
	{
		cout << "ServerVfd="<<iter->first << ",";
		vector<int>::iterator subiter = (iter->second).begin();
		while(subiter!= (iter->second).end())
		{
			cout << "PeerVfdList={"<< *subiter;
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
	cout << "Map NUmbers:"<<PeerList.size()<<endl;

	map< int, StructSock * >::iterator PeerIter = PeerList.begin();
	while(PeerIter!=PeerList.end())
	{
		cout << PeerIter->first;
		PeerIter++;
	}

}
