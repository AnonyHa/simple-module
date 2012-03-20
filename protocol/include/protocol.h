// $Id: protocol.h 32231 2008-03-25 06:47:43Z lfli $

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "data_format.h"
#include <map>
#include <string>

#define XYNET_ERROR_UNPACK_FORMAT	-1  //���ʱ���ݰ���ʽ����
#define XYNET_ERROR_UNPACK_SCRIPT	-2  //�ű�������ȷ�����ݰ�����

using namespace std;

typedef unsigned int VFD;

extern std::string cur_char;
//////////////////////////////////////////////////////////////////////////
// Զ�̵��ø�ʽ����
//////////////////////////////////
class proto_manager ;
class net_protocol {
	std::vector<fcall_base_arg *> _args ; // �����б�
	std::string _pack_func_name ;// ����ӿ����ƣ�������ʵ��
	int _unpack_ref ;			 // ����ӿ����ã��ű���ʵ��
	int _id ;					 // Э��id���й������Զ�����
	bool _is_maker;				 // �Ƿ�ΪԶ�̺���������
	proto_manager* _ProtoManager;
protected:
	int process_type(lua_State *L, const char* path);
	int convert_protocol(lua_State *L) ;
	int func_proc(lua_State *L) ;
	int arglist_proc(lua_State *L) ;
public:
	size_t _pack_count ;  			//����pack_func �Ĵ���������ͳ�ơ�
	size_t _unpack_count ;  		//����unpack_func �Ĵ���������ͳ�ơ�
	static size_t _total_pack_count ;
	static size_t _total_unpack_count ;

	net_protocol(int id, proto_manager* ProtoManager) ;
	~net_protocol() ;
	int get_id () { return _id ; } 

	int load(lua_State*L) ; //  ����Э���ʽ��ջ��ΪЭ���ʽ�����ļ�·��
	int unpack(lua_State*L, const byte* buf, int buf_len, int ext) ;
	int get_check_id(void);
	int marshal(lua_State * L, byte * buf, size_t buf_len) ;
	std::string& GetPackFuncName() {return _pack_func_name;};
	std::vector<fcall_base_arg *>& GetProtoArgs() {return _args;};
	proto_manager* GetProtoManager() {return _ProtoManager;};
	
protected:
	static int pack(lua_State* L) ; // ���Э���������Զ�ע����lua������й�Զ�̵�����ʹ��

};

//////////////////////////////////////////////////////////////////////////
// Э�������
////////////////////
// �������ݷ��͵Ļص�����
typedef int (*send_hook_t)(const byte* data, int data_len, unsigned int ext, int ismulticast, int mcpayloadlen);
class proto_manager {
	char* _for_maker;
	char* _for_caller;
	std::vector<net_protocol*> _s_protos;
	send_hook_t s_data_sender;  
	unsigned _static_protocol_count ;  //��̬��Э��ĸ���
	friend class net_protocol;
public:
	proto_manager(send_hook_t func);
	~proto_manager();

	int stat (lua_State * L) ;

	int add_arg_type(lua_State*L); // ջ��Ϊ���ݸ�ʽ�����ļ�·��
	int add_protocol(lua_State*L); // ջ��ΪЭ���ʽ�����ļ�·��
	int update_protocol(lua_State*L); // ����Э��
	int add_static_protocol(lua_State*L); //ֻ��add_protocol�ļ򵥷�װ
	int get_ptos_checkid(lua_State*L); // ��ȡ�����Э���У����
	void SetForMaker(char* ForMaker);
	void SetForCaller(char* ForCaller);
	char* GetForMaker(){return (!_for_maker)?(char*)"":_for_maker;};
	char* GetForCaller(){return (!_for_caller)?(char*)"":_for_caller;};
	std::vector<net_protocol*>& GetProtos() {return _s_protos;};
	//����ͽ���ĺ���
	send_hook_t GetSendHookFunc() {return s_data_sender;};
	int unpack_data(lua_State* L, const byte* buf, int buf_size, int ext);
};

bool InitProtocolLib(lua_State * L);

proto_manager* CreateNewProtoManager(string ProtoName, char* ForMaker, char* ForCaller,send_hook_t func);
extern map<string, proto_manager*> ProtoManagerMap;
#endif //_PROTOCOL_H
