#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include <stdio.h>

class MyBuf {
	private:
		int _BufSize;
		char* _Buf;
		char* _Head;
		char* _Tail;
		int GetBufLen() {return _Head-_Tail;};
		int GetRemainLen() {return _BufSize - GetBufLen();};
		int GetTailLen() {return _Buf+_BufSize-_Tail;};
	public:
		MyBuf(int BufSize=1024);
		~MyBuf();
		void InsertData(char* Input, int InputLen, int& RealLen);
		char* GetBufData(int DataLen, int& RealLen);
};

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

	if (RealLen != 0) memcpy(_Tail, Input, RealLen);		
}

char* MyBuf::GetBufData(int DataLen, int& RealLen)
{
	RealLen = (DataLen > GetBufLen())? GetBufLen():DataLen;
	char* DataHead = _Head;
	_Head = _Head + RealLen;
	return DataHead;
}

/////////////////////////////////////////////
#include <string>
#include <iostream>

using namespace std;

int main()
{
	MyBuf * Test = new MyBuf(2);
	string Buf("abcefg");
	int RealLen;
	Test->InsertData((char *)Buf.c_str(), (int)Buf.size() - 2, RealLen);

	cout <<"Get Data Len 3:"<<endl;
	int ReadLen;
	char* Output = Test->GetBufData(3,ReadLen);
	for (int i=0;i<3;i++)
		cout << Output[i];
	cout << "\tGet ReadLen:"<<ReadLen<<endl;

	return 0;
}
