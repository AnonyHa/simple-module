#include "packet_process.h"
#include <stdio.h>

clsPacketEncrypt::clsPacketEncrypt(clsEncrypt* EncryptClass)
{
	_EncryptClass = EncryptClass;
}

clsPacketEncrypt::~clsPacketEncrypt()
{
}

char* clsPacketEncrypt::EncryptPacket(char* Buf, int BufLen, int& RealLen)
{
	int BagLen = sizeof(int);
	unsigned char* tmp = new unsigned char[BagLen+BufLen];
	memcpy(tmp, &BufLen, BagLen);
	memcpy(tmp+BagLen, Buf, BufLen);	

	//执行加密函数
	int OutLen;
	char* OutBuf;
	if(_EncryptClass) 
	{
		OutBuf = _EncryptClass->Encrypt((char*)tmp, BagLen+BufLen, OutLen);
	}
	else
	{
		OutBuf = (char *)tmp;
		OutLen = BagLen + BufLen;
	}

	RealLen = OutLen;

	/*
	for (int i=0;i<OutLen;i++)
		printf("%02x ", OutBuf[i]);
	*/

	//调用数据直接写
	return OutBuf;
}

///////////////////////////////////////////////////////////////////////////

clsPacketDecrypt::clsPacketDecrypt(clsEncrypt* EncryptClass)
{
	_DecryptBuf = new MyBuf();	
	_EncryptClass = EncryptClass;
	_BagFlag = false;
}

clsPacketDecrypt::~clsPacketDecrypt()
{
	delete _DecryptBuf;
}

void clsPacketDecrypt::DecryptPacket(char* Buf,int BufLen)
{
	int DecLen;
	char* DecryptBuf;
	int RealLen = 0;

	if(_EncryptClass)
		DecryptBuf = _EncryptClass->Decrypt(Buf, BufLen, DecLen);		
	else
	{
		DecryptBuf = Buf;
		DecLen = BufLen;	
	}

	printf("==========\n");
	printf("Get Dec Len:%d\n", DecLen);

	if(DecLen > 0)
		_DecryptBuf->InsertData(DecryptBuf, DecLen, RealLen);			

	BagProcess();
}

void clsPacketDecrypt::BagProcess()
{
	while(1)
	{
		//没有取到原有的包长度
		if(!_BagFlag)
		{
			int BufLen;
			char* Head = _DecryptBuf->GetBufHead(BufLen);	
			if (BufLen < 4) return; 
			_BagLen = *(int *)Head;
			_BagFlag = true;

			int RealLen;
			_DecryptBuf->GetBufData(4, RealLen);
		}		
		else
		{
			printf("Get Some Data:%d %d\n", _DecryptBuf->GetBufLen(), _BagLen);
			if(_DecryptBuf->GetBufLen() < _BagLen) return; 
			int RealLen;	
			char* BagBuf = _DecryptBuf->GetBufData(_BagLen, RealLen);
			//调用包处理接口
			printf("Get A Bag,RealLen=%d\n", RealLen);
			for(int i=0;i<RealLen;i++)
				printf("%c", BagBuf[i]);
			printf("\n");

			_BagFlag = false;
		}
	}
}
