#ifndef __SIMPLE_PACKET_FUNC_H__
#define __SIMPLE_PACKET_FUNC_H__

#include "packet_interface.h"

#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

class SimplePacketFunc:public PacketInterface{
	public:
		bool PacketOnRead(int Vfd, char* Buf, int BufLen)
		{
			cout << "Vfd="<<Vfd<<"Get Msg:";
			for (int i=0; i< BufLen; i++)
				cout << Buf[i];
			cout <<endl;
			return true;
		};

		bool PacketOnError(int Vfd)
		{
			cout << "Vfd="<<Vfd<<" is Closed!"<<endl;
			return true;
		};
		bool PacketOnConnect(int Vfd)
		{
			cout << "Connect OK!"<<Vfd<<endl;
			char* TmpBuf="conn ok!";
			PacketOnWrite(Vfd, TmpBuf, strlen(TmpBuf));
			return true;
		};

};

#endif
