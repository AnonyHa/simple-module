#include "server_socket.h"
#include "socket_exception.h"
#include "event.h"

#include <iostream>
#include <string>

using namespace std;

int main(void)
{
	event_init();
	clsServerSocket* Server = new clsServerSocket(6666);
	try{
		Server->Start();
		cout << "Start Server At Port:"<<Server->GetPort()<<endl;
	}
	catch(SocketError& e)
	{
		cout << "Get Error:"<<e.GetMsg()<<endl;
	}

	event_dispatch();
	return 0;
}
