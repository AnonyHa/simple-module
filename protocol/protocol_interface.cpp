#include "protocol.h"
#include "socket_manager.h"
#include "packet_process.h"

class BagProcess{
	private:
		SocketManager* _Manager;
		clsPacketDecrypt* _PacketDecrypt;	
		clsPacketEncrypt* _PacketEncrypt;
		proto_manager* _ProtoManager;
		lua_State* _GlobalL;
	public:
		BagProcess();		
		~BagProcess();
		int RecvBag(char* Buf, int BufLen);
		bool BindToProtoManager(proto_manager* ProtoManager);
		//int SendBag();
};

BagProcess::BagProcess()
{
	
}

BagProcess::~BagProcess()
{
}

int BagProcess::RecvBag(char* Buf, int BufLen, int Vfd)
{
	return _ProtoManager->unpack_data(_GlobalL, (const unsigned char*)Buf, BufLen, Vfd);					
}

bool BagProcess::BindToProtoManager(proto_manager* ProtoManager)
{
	_ProtoManager = ProtoManager;
	//ProtoManager的HookSend函数的绑定
}
