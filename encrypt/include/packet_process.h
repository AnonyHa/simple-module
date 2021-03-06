#ifndef __PACKET_PROCESS_H__
#define __PACKET_PROCESS_H__

#include "mybuf.h"
#include "encrypt.h"

class clsPacketProcess {
};

/////////////////////////////////////////////
class clsPacketEncrypt{
	private:
		clsEncrypt* _EncryptClass;
	public:
		clsPacketEncrypt(clsEncrypt* EncryptClass=0);
		~clsPacketEncrypt();
		char* EncryptPacket(int Vfd, char* Buf, int BufLen,int& RealLen);
};

class clsPacketDecrypt{
	private:
		MyBuf* _DecryptBuf;
		unsigned int _BagLen;
		bool _BagFlag;
		void BagProcess(int Vfd);
		clsEncrypt* _EncryptClass;
	public:
		clsPacketDecrypt(clsEncrypt* EncryptClass=0);
		~clsPacketDecrypt();
		void DecryptPacket(int Vfd, char* Buf, int BufLen);
};
#endif
