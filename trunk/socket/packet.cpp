#include "packet.h"
#include <iostream>
#include <string>

using namespace std;

bool SimpleOutPacket::PacketOnRead(char* buf, int BufLen)
{
	cout << "Get Msg:"<<endl;
	for(int i=0; i<BufLen; i++)
		cout << buf[i];
	return true;
}

bool SimpleOutPacket::PacketOnWrite(char* buf, int BufLen)
{
	return true;
}
