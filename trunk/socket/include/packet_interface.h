#ifndef __PACKET_INTERFACE_H__
#define __PACKET_INTERFACE_H__

#include "socket_manager.h"

//extern SocketManager* Manager;

class PacketInterface{
	public:
		bool PacketOnWrite(int ToVfd, char* Buf, int BufLen) {
			//return Manager->PeerVfdOnWrite(ToVfd, Buf, BufLen);
			return true;
		};
		virtual bool PacketOnRead(int Vfd, char* Buf, int BufLen)=0;
		virtual bool PacketOnError(int Vfd)=0;
};

#endif
