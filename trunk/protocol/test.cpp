#include "protocol.h"
#include "string.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "mbuf.h"
#include "stdio.h"

/*
void ctox(unsigned char c)
{
    char str[10];
    sprintf(str,"%02x")
}
*/

int HookSend (const byte len, unsigned int fd,int ismulticast = 0, int mcpayloadlen = 0)
{
    printf("the len is:%d\n",len);
    for(int i=0;i<len;i++)
        printf("%02x ",buf[i]);
    return 0;
}

std::string bytecur_char = "client";

proto_manager * pm = new proto_manager(HookSend) ;


int InitPtoManager(lua_State * L)
{
	if (pm)
		delete pm;	
	pm = new proto_manager(HookSend);
	return 0;
}



int cishi(lua_State* L)
{
    printf("the lib is ok!\n");
    return 0;
}

static const luaL_Reg  protolib [] = {
        { "add_arg_type", proto_manager::add_arg_type},
        { "add_protocol", proto_manager::add_protocol},
        { "update_protocol", proto_manager::update_protocol},
        { "add_static_protocol", proto_manager::add_static_protocol},
	    { "init_pto", InitPtoManager},
        { "get_check_sum", proto_manager::get_ptos_checkid},
        { "stat", proto_manager::stat},
        {"cishi",cishi},
        { NULL, NULL}
} ;

int main(void)
{
    lua_State* L = luaL_newstate();
    OutSockBuf = MbufNew(0xf000);
   
    luaL_openlibs(L);
    luaL_register(L,"proto",protolib);
    luaL_dofile(L,"test.lua");
   
   /* 
   char buf[20];
   unsigned char c;
   c=0x01;
   memcpy(buf,&c,1);
   c=0x02;
   memcpy(buf+1,&c,1);
   c=0x73; 
   memcpy(buf+2,&c,1); 
   c=0x02;
   memcpy(buf+3,&c,1);
   c=0x31;
   memcpy(buf+4,&c,1);
   printf("\nconstruct the buf:\n");
   for(int i=0;i<5;i++)
       printf("%02x ",buf[i]);
    printf("\n");
   pm->unpack_data(L,(const byte*)buf,5,1);
    */
   return 0;
}
