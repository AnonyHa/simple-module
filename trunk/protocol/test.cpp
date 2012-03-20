#include "protocol.h"
#include "string.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "stdio.h"

#include <bitset>
#include <iostream>

using namespace std;

/*
void ctox(unsigned char c)
{
    char str[10];
    sprintf(str,"%02x")
}
*/

int HookSend (const byte* buf, int len, unsigned int fd,int ismulticast = 0, int mcpayloadlen = 0)
{
    printf("the len is:%d\n",len);
    for(int i=0;i<len;i++)
        printf("%02x ",buf[i]);
	printf("\n");
    return 0;
}

int HookSend1 (const byte* buf, int len, unsigned int fd,int ismulticast = 0, int mcpayloadlen = 0)
{
    printf("the 11111111111 len is:%d\n",len);
    for(int i=0;i<len;i++)
        printf("%02x ",buf[i]);
	printf("\n");
    return 0;
}



std::string cur_char = "client";

int main(void)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
	InitProtocolLib(L);

	proto_manager* Pobj = CreateNewProtoManager(string("cishi"), (char*)"for_maker",(char*)"for_caller",HookSend);
	proto_manager* Pobj1 = CreateNewProtoManager(string("cishi_test"), (char*)"for_maker_test",(char*)"for_caller_test",HookSend1);
    luaL_dofile(L,"test.lua");

	byte buf[9];
	unsigned char m;
	m=0x01;
	memcpy(buf, &m, 1);
	m=0x04;
	memcpy(buf+1, &m, 1);
	m=0xc1;
	memcpy(buf+2, &m, 1);
	m=0x07;
	memcpy(buf+3, &m, 1);

	m=0x05;
	memcpy(buf+4, &m, 1);

	m=0x7a;
	memcpy(buf+5, &m, 1);

	m=0x65;
	memcpy(buf+6, &m, 1);

	m=0x72;
	memcpy(buf+7, &m, 1);

	m=0x6f;
	memcpy(buf+8, &m, 1);

	Pobj->unpack_data(L, (const byte *)buf, 9, 6666);
	printf("Task End!\n");
	Pobj1->unpack_data(L, (const byte *)buf, 9, 6667);

    return 0;
}
