#ifndef __SIMPLE_PACKET_FUNC_H__
#define __SIMPLE_PACKET_FUNC_H__

#include "packet_interface.h"

#include <iostream>
#include <string>

using namespace std;

class SimplePacketFunc:public PacketInterface{
	public:
		void PacketOnWrite(int ToVfd, char* Buf, int BufLen) {};
		bool PacketOnRead(int Vfd, char* Buf, int BufLen)
		{
			cout << "Vfd="<<Vfd<<"Get Msg:";
			for (int i=0; i< BufLen; i++)
				cout << Buf[i];
			cout <<endl;
		};

		bool PacketOnError(int Vfd)
		{
			cout << "Vfd="<<Vfd<<" is Closed!"<<endl;
		};

};

#endif
