#ifndef __SIMPLE_SERVER_FUNC_H__
#define __SIMPLE_SERVER_FUNC_H__

#include "packet_interface.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

class SimpleServerFunc:public PacketInterface{
	public:
		bool PacketOnRead(int Vfd, char * Buf, int BufLen)
		{
			cout << "Get Vfd="<<Vfd<<" Msg:"<<endl;
			for(int i=0;i<BufLen;i++)
			{
				cout << Buf[i];
			}
			cout << endl;
			return true;
		};

		bool PacketOnError(int Vfd)
		{
			cout<<"Close Vfd="<<Vfd<<endl;
			return true;
		};
		bool PacketOnConnect(int Vfd)
		{
			char* Buf="Hello World";
			return PacketOnWrite(Vfd, Buf, strlen(Buf));
		};
};

#endif