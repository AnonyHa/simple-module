class PacketInterface{
	public:
		virtual bool PacketOnRead(char* Buf, int BufLen)=0;
		virtual bool PacketOnWrite(char* Buf, int BufLen)=0;
};

class SimpleOutPacket:public PacketInterface
{
	private:
	public:
		bool PacketOnRead(char* buf, int BufLen);
		bool PacketOnWrite(char* buf, int BufLen);
};

