#include "socket_manager.h"
#include "event.h"
#include "socket_exception.h"
#include "simple_client_func.h"

#include <iostream>
#include <string>

using namespace std;
SocketManager* Manager;

/************  ²âÊÔÓÃ *****************/
int main(void)
{
	event_init();

	Manager = new SocketManager();
	
	try{
		SimplePacketFunc* SimpleFunc = new SimplePacketFunc();
		clsClientSocket* Test = Manager->CreateClientSocket(string("192.168.10.48"), 6666, SimpleFunc);
	}
	catch(SocketError& e)
	{
		cout << "ErrMsg:" << e.GetMsg() << endl;
	}

	while(1)
	{
		event_dispatch();
	}

	return 0;
}
