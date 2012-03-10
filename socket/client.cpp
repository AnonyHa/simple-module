#include "client_socket.h"
#include "event.h"
#include "socket_exception.h"

#include <iostream>
#include <string>

/************  ²âÊÔÓÃ *****************/
int main(void)
{
	event_init();

	clsClient* Test = new clsClient(string("127.0.0.1"), 6666);
	try{
		Test->Connect();
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
