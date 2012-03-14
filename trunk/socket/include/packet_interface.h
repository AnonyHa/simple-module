#ifndef __PACKET_INTERFACE_H__
#define __PACKET_INTERFACE_H__

class PacketInterface{
	public:
		bool PacketOnWrite(int ToVfd, char* Buf, int BufLen);
		virtual bool PacketOnRead(int Vfd, char* Buf, int BufLen)=0;
		virtual bool PacketOnError(int Vfd)=0;
		virtual bool PacketOnConnect(int Vfd)=0;//���ڷ����������������û����ӣ�����ǿͻ��ˣ����Ǹո������ϵ�ʱ�����
};

#endif
