class PacketInterface{
	protect:
		clsClientSocket* _ClientObj;
	public:
		virtual bool PacketOnRead(char* Buf, int BufLen)=0;
		virtual bool PacketOnWrite(char* Buf, int BufLen)=0;
		virtual bool PacketOnError()=0;
		bool RegistClient(clsClientSocket* ClientObj) {_ClientObj = ClientObj};
};

class SimpleOutPacket:public PacketInterface
{
	private:
	public:
		bool PacketOnRead(char* buf, int BufLen);
		bool PacketOnWrite(char* buf, int BufLen);
		bool PacketOnError();
};

