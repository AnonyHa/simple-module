#include "packet_process.h"
#include "encrypt.h"
#include <stdio.h>

int main()
{
	SimpleXOR* encrypt = new SimpleXOR();
	SimpleXOR* decrypt = new SimpleXOR();

	clsPacketEncrypt* EncryptObj = new clsPacketEncrypt(encrypt);	
	clsPacketDecrypt* DecryptObj = new clsPacketDecrypt(decrypt);

	const char* Input="abcdef";
	char* Output;
	int OutLen;
	Output = EncryptObj->EncryptPacket((char *)Input, 6, OutLen);
	printf("Encrypt OutLen:%d\n", OutLen);
	DecryptObj->DecryptPacket(Output, OutLen);
	printf("Decrypt OutLen:%d\n", OutLen);
}
