#include "socket_manager.h"
#include "socket_exception.h"
#include "event.h"
#include "simple_packet_func.h"

#include <iostream>
#include <string>

using namespace std;

SocketManager* Manager;

int main(void)
{
	event_init();

	Manager = new SocketManager();
	try{
		SimplePacketFunc* Test = new SimplePacketFunc();
		clsServerSocket* Server = Manager->CreateServerSocket(6666, Test);
	}
	catch(SocketError& e)
	{
		cout << "Get Error:"<<e.GetMsg()<<endl;
	}

	event_dispatch();
	return 0;
}
