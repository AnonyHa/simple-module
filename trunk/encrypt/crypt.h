// encode_decode_test.cpp : Defines the entry point for the console application.

#ifndef __CRYPT__
#define __CRYPT__

#define DIC_SIZE 256
// 简单加密，暂用版本
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

class xcode {
	unsigned char dic_i;
	unsigned char* dic;
public:
	xcode() { dic = new unsigned char[DIC_SIZE];}
	xcode(const xcode& Instance)
	{
		dic = new unsigned char[DIC_SIZE];
		memcpy(dic, Instance.dic, DIC_SIZE);
		dic_i = Instance.dic_i;
	}	
	~xcode(){ delete [] dic;}
	
	void set_seed(int sd) {
		dic_i = 0;
		for (int i=0; i<256; ++i) {
			dic[i] = (unsigned char)(sd%256);
			dic[i] ^= dic[i] ^ (unsigned char)sd;
			sd = (sd<<3)+(sd>>3)+sd;
		}
	}

	void output(){
		for (int i=0; i<DIC_SIZE; i++)
			printf("%02x ", dic[i]);
	}

	void exchange(unsigned char c1, unsigned char c2){
		unsigned char tmp = dic[c1];
		dic[c1] = dic[c2];
		dic[c1] = tmp;
	}

	unsigned char xmake(unsigned char c) { 
		++dic_i;
		exchange(dic_i, c);
		return dic[dic_i]^c;
	}

};

#endif

#ifdef __TEST__
int _tmain(int argc, _TCHAR* argv[])
{
	const int data_len = 3;
	char in_buf [data_len];
	char out_buf [data_len];
	char org_buf [data_len];
	
	xcode encode, decode;
	encode.set_seed(10);
	decode.set_seed(10);

	in_buf[0] = 90;
	in_buf[1] = 10;
	in_buf[2] = 20;

	printf("\n\nin:\n");
	for (int i=0; i<sizeof(in_buf); ++i) {
		printf("%03d ", in_buf[i]);
		out_buf[i] = encode.xmake(in_buf[i]);
	}

	printf("\n\nout:\n");
	for (int i=0; i<sizeof(out_buf); ++i) {
		printf("%03d ", out_buf[i]);
	}

	printf("\n\norg:\n");
	for (int i=0; i<sizeof(org_buf); ++i) {
		org_buf[i] = decode.xmake(out_buf[i]);
		printf("%03d ", org_buf[i]);
	}

	return 0;
}
#endif
