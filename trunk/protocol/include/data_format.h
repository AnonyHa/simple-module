// $Id: data_format.h 49734 2008-08-08 04:22:20Z zhj $

#ifndef _DATA_FORMAT_H
#define _DATA_FORMAT_H

#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <cstdarg>


//#include "util.hpp"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
//#include "lstate.h"
//#include <lua.hpp>


/*
#define print_error(L, fmt, args...) do {_ERROR(fmt, ##args); } while (0) 
	
#define print_warn(L, fmt, args...) do {_WARN(fmt, ##args); } while (0)

#define print_runtime_error(L, fmt, args...) do { _RUNTIME_ERROR(fmt, ##args) ; } while (0)


#define raise_error(L, fmt, args...) 	do {			\
	_CRIT(fmt, ##args);					\
	_RUNTIME_ERROR (fmt, ##args) ;				\
	if (L->errorJmp)					\
		luaL_error(L, fmt, ##args);			\
	} while (0)					
	
*/	

#define check_buf(data_len, buf_len) 	do {			\
		if ((data_len) > (buf_len)) {			\
	 		return 0;				\
        }						\
	 } while (0)				

typedef unsigned char byte;
static const int PROTO_OK = 0;
static const int PROTO_ERROR = -1;
int read_file(const char* fpath, char* buf, int buf_len) ;

////////////////////////////////////////////////////////////////////////
// luaԶ�̺���������������
//////////////////////////////////
class fcall_base_arg {
protected:
	std::string _type;
	std::string _hashstr;
public:
	fcall_base_arg() { _type = "base"; _hashstr = "base";}
	virtual ~fcall_base_arg(){};
	// ��data_inָ����lua����תΪ����������
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len) = 0;
	// �Ӷ������������н����ǰ������ѹ��luaջ
	virtual int unpack(lua_State*L, const byte* buf, int buf_len) = 0 ;
	bool check_type(const char* type_name) { return (_type == type_name); }
	const char * get_type () { return _type.c_str () ; }
	const char * get_key() {return _type.c_str();}
	const char * get_hashstr() {return _hashstr.c_str();}
};

// �����Ͳ���(�䳤����)
class fcall_number_arg : public fcall_base_arg  {
	struct number_type {
		byte is_negative:1; //�Ƿ�Ϊ����
		byte len:7;
	 }__attribute__ ((aligned(1))) ;
public:
	fcall_number_arg() { _type = "number"; _hashstr = "number";}
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State*L, const byte* buf, int buf_len);
};

// byte�Ͳ���(��������, ���ֽ�)
class fcall_byte_arg : public fcall_base_arg  {
public:
	fcall_byte_arg() { _type = "byte";_hashstr = "byte";}
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State*L, const byte* buf, int buf_len);
};

// word�Ͳ���(��������, ˫�ֽ�)
class fcall_word_arg : public fcall_base_arg  {
public:
	fcall_word_arg() { _type = "word";_hashstr = "word";}
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State*L, const byte* buf, int buf_len);
};

// dword�Ͳ���(��������, ���ֽ�)
class fcall_dword_arg : public fcall_base_arg  {
public:
	fcall_dword_arg() { _type = "dword"; _hashstr = "dword";}
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State*L, const byte* buf, int buf_len);
};

// �ַ����Ͳ���
class fcall_string_arg : public fcall_base_arg {
public:
	fcall_string_arg() { _type = "string"; _hashstr = "string"; }
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State*L, const byte* buf, int buf_len);
};

// ����Ƕ��ʽ����
class fcall_table_arg : public fcall_base_arg {
	enum tb_op_code { tb_begin=0, tb_end, push_value };
	struct table_opcode {
		tb_op_code op;
		int key_ref;			   // ��lua������ʽ��¼key����߽����ٶ�
		fcall_base_arg* value_arg; // ֱ�Ӽ�¼����������,Ϊ�ձ�ʾΪδ֪table���� 
		table_opcode(tb_op_code, int, fcall_base_arg* arg);
	};
	// ���ڼ�¼table��key����ʽ��Ϣ�����ü�¼����table opcode�ķ�ʽ
	std::vector<table_opcode> _opcodes; 
	static const int max_table_deep = 10;// table�����Ƕ����ȣ���ֹ����ʱջ���
	int _cur_table_deep; 
protected:
	int _read_format(lua_State*L);	// ���ز�ת������table
	void _travel_table(lua_State*L, int table_index); // ����table��ת��Ϊopcode
public:
	fcall_table_arg(lua_State*L); // ջ������Ϊ��ʽ�����ļ�·��
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State*L, const byte* buf, int buf_len);
};

// �����Ͳ���
class fcall_array_arg : public fcall_base_arg {
	fcall_base_arg* _base_packer;
	int _size;
public:
	fcall_array_arg(const char* base_type, int size);
	virtual int pack(lua_State*L, int data_in, byte* buf, int buf_len);
	virtual int unpack(lua_State*L, const byte* buf, int buf_len);
};

//////////////////////////////////////////////////////////////////////////
// ���ݽ���������
class fcall_arg_manager {
	static std::vector<fcall_base_arg*> _s_args;
public:
	static void init(void);
	static void destruct(void);
	static void add_table_args(lua_State*L);
	static void add_arg(fcall_base_arg* arg) { _s_args.push_back(arg); }
	static fcall_base_arg* get_arg(int arg_id);
	static fcall_base_arg* get_arg(const char* type_name);
};

#endif //_DATA_FORMAT_H
