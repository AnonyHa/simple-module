#ifndef __PACKET_INTERFACE_H__
#define __PACKET_INTERFACE_H__

class PacketInterface{
	public:
		bool PacketOnWrite(int ToVfd, char* Buf, int BufLen);
		virtual bool PacketOnRead(int Vfd, char* Buf, int BufLen)=0;
		virtual bool PacketOnError(int Vfd)=0;
		virtual bool PacketOnConnect(int Vfd)=0;//对于服务器，则是有新用户连接，如果是客户端，则是刚刚连接上的时候调用
};

#endif
