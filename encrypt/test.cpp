#include "packet_process.h"
#include <stdio.h>

int main()
{
	clsPacketEncrypt* EncryptObj = new clsPacketEncrypt();	
	clsPacketDecrypt* DecryptObj = new clsPacketDecrypt();

	char* Input="abcdef";
	char* Output;
	int OutLen;
	Output = EncryptObj->EncryptPacket(Input, 4, OutLen);
	//printf("out Address %x\n", Output);
	//DecryptObj->DecryptPacket(Output, OutLen);
}
