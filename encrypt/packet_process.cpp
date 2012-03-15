#include "mybuf.h"

class clsPacketProcess {
};

class clsPacketEncrypt{
	private:
		MyBuf* _EncryptBuf;
	public:

};

class clsPacketDecrypt{
	private:
		MyBuf* _DecryptBuf;
	public:
		clsPacketDecrypt();
		~clsPacketDecrypt();
		void DecryptPacket(char* Buf, int BufLen);
};

clsPacketDecrypt::clsPacketDecrypt()
{
	_EncryptBuf = new MyBuf();	
}

clsPacketDecrypt::~clsPacketDecrypt()
{
	delete _EncryptBuf;
}

void clsPacketDecrypt::DecryptPacket(char* Buf,int BufLen)
{
	int DecLen;
	char* DecryptBuf = XXX (Buf, BufLen, DecLen);		
	if(DecLen > 0)
	{
		int RealLen;
		_DecryptBuf->InsertData(DecryptBuf, DecLen, RealLen);			
	}

	//后续分包处理
}

