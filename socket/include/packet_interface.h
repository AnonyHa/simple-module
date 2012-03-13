#ifndef __PACKET_INTERFACE_H__
#define __PACKET_INTERFACE_H__

class PacketInterface{
	public:
		void PacketOnWrite(int ToVfd, char* Buf, int BufLen) {};
		virtual bool PacketOnRead(int Vfd, char* Buf, int BufLen);
		virtual bool PacketOnError(int Vfd);
};

#endif
