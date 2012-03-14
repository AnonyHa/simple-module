#include "packet_interface.h"
#include "socket_manager.h"

extern SocketManager* Manager;

bool PacketInterface::PacketOnWrite(int ToVfd, char* Buf, int BufLen)
{
	return Manager->PeerVfdOnWrite(ToVfd, Buf, BufLen);
}
