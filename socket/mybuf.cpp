#include "mybuf.h"

#include <cstdlib>
#include <cstring>

using namespace std;

MyBuf::MyBuf(int BufSize)
{
	_BufSize = BufSize;
	_Buf = (char*)malloc(_BufSize);
	_Head = _Buf;
	_Tail = _Buf;
}

MyBuf::~MyBuf()
{
	free(_Buf);	
}

void MyBuf::InsertData(char* Input, int InputLen, int& RealLen)
{
	int BufLen = GetBufLen();
	RealLen = InputLen;
	if (InputLen > GetTailLen())
	{
		//ÒÆ¶¯
		if (_Buf != _Head)
		{
			memmove(_Buf, _Head, BufLen); 
			_Head = _Buf;
			_Tail = _Head + BufLen;
		}
	}

	if (InputLen > GetRemainLen())
	{
		//À©³äbuf
		int NeedLen = BufLen + InputLen;
		while(_BufSize < NeedLen)
		{
			_BufSize = _BufSize * 2;
		}

		_Buf = (char *)realloc(_Head, NeedLen); 
		if (!_Buf) RealLen = 0;
		_Head = _Buf;
		_Tail = _Head + BufLen;
	}

	if (RealLen != 0) 
	{
		memcpy(_Tail, Input, RealLen);
		_Tail = _Tail + RealLen;
	}	
}

char* MyBuf::GetBufData(int DataLen, int& RealLen)
{
	RealLen = (DataLen > GetBufLen())? GetBufLen():DataLen;
	char* DataHead = _Head;
	_Head = _Head + RealLen;
	return DataHead;
}

char* MyBuf::GetBufHead(int& BufLen)
{
	BufLen = GetBufLen();
	return _Head;
}

