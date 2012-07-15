#include <iostream>
#include <unistd.h>
#include "mybuf.h"
#include <map>

#include <stdio.h>
#include <assert.h>

using namespace std;

#define HEADER 1
#define BODY 2

#define PACKET_MAX_LEN 4096

// 需要依赖 缓冲区数组，需要依赖Body处理类
class PacketHeaderProcess{
	public:
		PacketHeaderProcess();
		~PacketHeaderProcess(){delete _VfdBuf;};
		int ProcessHeaderData(int Vfd, char* Buf, int BufLen);

		void ReadData(int Vfd, char* Buf, int BufLen);
		char* WriteData(int Vfd, char* Buf, int BufLen);

	private:
		int ReadOnceData(int Vfd, char* Buf, int BufLen);

		int _status;
		int _NeedBufLen;
		int _Header;
		int _ProcessVfd;
		MyBuf* _VfdBuf;
};

PacketHeaderProcess::PacketHeaderProcess()
{
	_status = HEADER;
	_NeedBufLen = 4;
	_ProcessVfd = -1;
	_VfdBuf = new MyBuf(PACKET_MAX_LEN);
}

int PacketHeaderProcess::ReadOnceData(int Vfd, char* Buf, int BufLen)
{
	int ProcessDataLen = 0;
	if (_status == HEADER)
	{
		_ProcessVfd = Vfd;
		unsigned char* _HeaderStart = (unsigned char *)(&(_Header) + (4-_NeedBufLen));
		ProcessDataLen = BufLen > _NeedBufLen? _NeedBufLen:BufLen;
		memcpy(_HeaderStart, Buf, ProcessDataLen);
		if (_NeedBufLen <= BufLen)
		{
			_status = BODY;
			_NeedBufLen = _Header;
		}
		else
		{
			_NeedBufLen -= BufLen;
		}
	}
	else
	{
		assert(Vfd == _ProcessVfd);
		ProcessDataLen = BufLen > _NeedBufLen? _NeedBufLen:BufLen;
		int RealLen;
		_VfdBuf->InsertData(Buf, ProcessDataLen, RealLen);
		assert(RealLen == ProcessDataLen);

		if (_NeedBufLen <= BufLen)
		{
			// 实际处理数据??
			int TotalLen = 0;
			char* Data = _VfdBuf->GetBufHead(TotalLen);	
			//  Data, TotalLen
			_status = HEADER;
			_NeedBufLen = 4;
			_ProcessVfd = -1;
		}
		else
		{
			_NeedBufLen -= BufLen;
		}
	}

	return ProcessDataLen;
}

void PacketHeaderProcess::ReadData(int Vfd, char* Buf, int BufLen)
{
	int RemainLen = BufLen;
	char* TmpBuf = Buf;
	while (RemainLen > 0)
	{
		int RetLen = ReadOnceData(Vfd, TmpBuf, RemainLen);
		RemainLen -= RetLen;
		TmpBuf += RetLen;
	}
}

char* PacketHeaderProcess::WriteData(int Vfd, char* Buf, int BufLen)
{
	int NewBufLen = BufLen + 4;
	int HeaderLen = BufLen;	
	char* SendData = (char*)malloc(NewBufLen);

	memcpy(SendData, (char *)&(HeaderLen), 4);
	memcpy(SendData + 4, Buf, BufLen);
	//调用实际的发包函数
	return SendData;
}

///////////////////////////////////////////
#ifdef __UNIT_TEST__
int main()
{
	PacketHeaderProcess* Test = new PacketHeaderProcess();	
	char* Input="Hello World!";
	char* p = Test->WriteData(1, (char*)Input, strlen(Input));
	Test->ReadData(1, p, strlen(Input) + 4);
}
#endif
