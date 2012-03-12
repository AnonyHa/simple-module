#include "socket_manager.h"
#include "socket_exception.h"
#include "event.h"

#include <iostream>
#include <string>

using namespace std;

SocketManager* Manager;

int main(void)
{
	event_init();

	Manager = new SocketManager();
	try{
		clsServerSocket* Server = Manager->CreateServerSocket(6666);
	}
	catch(SocketError& e)
	{
		cout << "Get Error:"<<e.GetMsg()<<endl;
	}

	event_dispatch();
	return 0;
}
