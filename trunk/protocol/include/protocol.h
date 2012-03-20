// $Id: protocol.h 32231 2008-03-25 06:47:43Z lfli $

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "data_format.h"
#include <map>
#include <string>

#define XYNET_ERROR_UNPACK_FORMAT	-1  //解包时数据包格式出错
#define XYNET_ERROR_UNPACK_SCRIPT	-2  //脚本处理正确的数据包出错

using namespace std;

typedef unsigned int VFD;

extern std::string cur_char;
//////////////////////////////////////////////////////////////////////////
// 远程调用格式描述
//////////////////////////////////
class proto_manager ;
class net_protocol {
	std::vector<fcall_base_arg *> _args ; // 参数列表
	std::string _pack_func_name ;// 打包接口名称，引擎内实现
	int _unpack_ref ;			 // 解包接口引用，脚本内实现
	int _id ;					 // 协议id，有管理器自动分配
	bool _is_maker;				 // 是否为远程函数定义者
	proto_manager* _ProtoManager;
protected:
	int process_type(lua_State *L, const char* path);
	int convert_protocol(lua_State *L) ;
	int func_proc(lua_State *L) ;
	int arglist_proc(lua_State *L) ;
public:
	size_t _pack_count ;  			//调用pack_func 的次数，用于统计。
	size_t _unpack_count ;  		//调用unpack_func 的次数，用于统计。
	static size_t _total_pack_count ;
	static size_t _total_unpack_count ;

	net_protocol(int id, proto_manager* ProtoManager) ;
	~net_protocol() ;
	int get_id () { return _id ; } 

	int load(lua_State*L) ; //  加载协议格式，栈顶为协议格式描述文件路径
	int unpack(lua_State*L, const byte* buf, int buf_len, int ext) ;
	int get_check_id(void);
	int marshal(lua_State * L, byte * buf, size_t buf_len) ;
	std::string& GetPackFuncName() {return _pack_func_name;};
	std::vector<fcall_base_arg *>& GetProtoArgs() {return _args;};
	proto_manager* GetProtoManager() {return _ProtoManager;};
	
protected:
	static int pack(lua_State* L) ; // 完成协议打包，被自动注册至lua虚拟机中供远程调用者使用

};

//////////////////////////////////////////////////////////////////////////
// 协议管理器
////////////////////
// 用于数据发送的回调函数
typedef int (*send_hook_t)(const byte* data, int data_len, unsigned int ext, int ismulticast, int mcpayloadlen);
class proto_manager {
	char* _for_maker;
	char* _for_caller;
	std::vector<net_protocol*> _s_protos;
	send_hook_t s_data_sender;  
	unsigned _static_protocol_count ;  //静态的协议的个数
	friend class net_protocol;
public:
	proto_manager(send_hook_t func);
	~proto_manager();

	int stat (lua_State * L) ;

	int add_arg_type(lua_State*L); // 栈顶为数据格式描述文件路径
	int add_protocol(lua_State*L); // 栈顶为协议格式描述文件路径
	int update_protocol(lua_State*L); // 更新协议
	int add_static_protocol(lua_State*L); //只是add_protocol的简单封装
	int get_ptos_checkid(lua_State*L); // 获取已添加协议的校验码
	void SetForMaker(char* ForMaker);
	void SetForCaller(char* ForCaller);
	char* GetForMaker(){return (!_for_maker)?(char*)"":_for_maker;};
	char* GetForCaller(){return (!_for_caller)?(char*)"":_for_caller;};
	std::vector<net_protocol*>& GetProtos() {return _s_protos;};
	//打包和解包的函数
	send_hook_t GetSendHookFunc() {return s_data_sender;};
	int unpack_data(lua_State* L, const byte* buf, int buf_size, int ext);
};

bool InitProtocolLib(lua_State * L);

proto_manager* CreateNewProtoManager(string ProtoName, char* ForMaker, char* ForCaller,send_hook_t func);
extern map<string, proto_manager*> ProtoManagerMap;
#endif //_PROTOCOL_H
