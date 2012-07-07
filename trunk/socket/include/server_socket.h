#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

#include "packet_interface.h"

class clsServerSocket{
	private:
		int _Vfd;
		int _Port;
		int _MaxNumber;
		PacketInterface* _PacketClass;
	public:
		clsServerSocket(int Port, PacketInterface* PacketClass = 0):_MaxNumber(1024) {_Port=Port;_PacketClass = PacketClass;};
		void Start();
		int GetVfd() {return _Vfd;};
		int GetPort() {return _Port;};
		PacketInterface* GetPacketInterface() {return _PacketClass;};
};

#endif
