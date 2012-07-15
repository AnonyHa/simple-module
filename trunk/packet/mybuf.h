#ifndef __MYBUF_H__
#define __MYBUF_H__

class MyBuf {
	private:
		int _BufSize;
		char* _Buf;
		char* _Head;
		char* _Tail;
		int GetRemainLen() {return _BufSize - GetBufLen();};
		int GetTailLen() {return _Buf+_BufSize-_Tail;};
	public:
		MyBuf(int BufSize=1024);
		~MyBuf();
		void InsertData(char* Input, int InputLen, int& RealLen);
		char* GetBufData(int DataLen, int& RealLen);
		char* GetBufHead(int& BufLen);
		int GetBufLen() {return _Tail-_Head;};
};

#endif
