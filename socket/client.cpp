#include "socket_manager.h"
#include "event.h"
#include "socket_exception.h"

#include <iostream>
#include <string>

using namespace std;
SocketManager* Manager;

/************  ������ *****************/
int main(void)
{
	event_init();

	Manager = new SocketManager();
	
	try{
		clsClientSocket* Test = Manager->CreateClientSocket(string("192.168.10.48"), 6666);
	}
	catch(SocketError& e)
	{
		cout << "Vfd:" << e.GetVfd() << "\tErrMsg:" << e.GetMsg() << endl;
	}

	while(1)
	{
		event_dispatch();
	}

	return 0;
}
