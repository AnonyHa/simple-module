#ifndef __BAG_PROCESS_H__
#define __BAG_PROCESS_H__

//#include "link_manager.h"
#include "socket_manager.h"
#include "packet_process.h"
#include "protocol.h"

#include <stdio.h>

class clsBagProcess{
	private:
		clsPacketEncrypt* _Enc;
		clsPacketDecrypt* _Dec;
		proto_manager* _PtoManager;
		lua_State* GlobalL;
	public:
		clsBagProcess();
		~clsBagProcess();
		void RecvBag(int Vfd, char* Buf, int BufLen);
		static int clsBagProcess::HookSend (const byte *buf,  int len, unsigned int fd,int ismulticast = 0, int mcpayloadlen = 0);
};

clsBagProcess::clsBagProcess(lua_State* L, clsPacketEncrypt* Enc, clsPacketDecrypt* Dec, proto_manager* PtoManager)
{
	//RegisterDec2PtoManager(Dec, PtoManager);
	//RegisterPtoManager2Enc(PtoManager, Enc);	
	_GlobalL = L;
	_Enc = Enc;
	_Dec = Dec;
	_PtoManager = PtoManager;
}

void clsBagProcess::RecvBag(int Vfd, char* Buf, int BufLen)
{
	_PtoManager->unpack_data(_GlobalL, (const unsigned char*)Buf, BufLen, Vfd);		
}


static int clsBagProcess::HookSend (const byte *buf,  int len, unsigned int fd,int ismulticast, int mcpayloadlen)
{	
	printf("it is OK!\n");
}
#endif
