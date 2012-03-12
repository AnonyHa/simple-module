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

bool SocketManager::AddServerPeerVfdList(int PeerVfd, clsPeerPoint* PeerObj, clsServerSocket* ServerObj)
{
	if (!PeerList.count(PeerVfd)) 
	{
		StructSock * tmp = new StructSock();
		tmp->InitServerStruct(PeerObj, ServerObj);
		PeerList[PeerVfd] = tmp;
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
