// $Id: protocol.cpp 91636 2009-08-19 08:34:12Z lfwu $
//#include "gamer.h"
//#include "log.h"
#include "protocol.h"

//#include "mbuf.h"
//#include "lbacktrace.h"
//#include "macros.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <map>

typedef unsigned char byte;
#define MAXTONETD 0x8000

extern int HookSend (const byte *buf,  int len, unsigned int fd,int ismulticast = 0, int mcpayloadlen = 0);

using namespace std;
map<string, proto_manager*> ProtoManagerMap;

//////////////////////////////////////////////////////////////////////////
// hash字符串，用于协议校验
static int calc_hashnr(const char *key, size_t length) 
{ 
	int nr=1, nr2=4; 
	while (length--) 
	{ 
		nr^= (((nr & 63)+nr2)*((int) (char) *key++))+ (nr << 8); 
		nr2+=3; 
	} 
	return nr; 
} 

//////////////////////////////////////////////////////////////////////////	
static const  std::string client = "client";
static const  std::string server = "server";
// 定义网络协议描述信息中的关键字
static const char func_name[] = "func_name";
static const char arg_list[] = "arg_list";

size_t net_protocol::_total_pack_count = 0 ;
size_t net_protocol::_total_unpack_count = 0 ;

net_protocol::net_protocol(int id, proto_manager* ProtoManager):_args(), _pack_func_name()
{
	_id = id;
	_is_maker = false;
	_unpack_ref = LUA_NOREF;
	_pack_count = 0 ;
	_unpack_count = 0 ;

	_ProtoManager = ProtoManager;
}

net_protocol::~net_protocol()
{
	_args.clear();
}

int net_protocol::load(lua_State *L)
{
	// 栈顶为协议描述文件路径
	int top = lua_gettop(L);
	// 依据文件名解析协议类型
	if (process_type(L, luaL_checkstring(L, -1)) == PROTO_ERROR) {
		return PROTO_ERROR;
	}
	
	// 加载协议内容
	const int buf_len = 100 * 1024;
	char read_buf[buf_len];
	const char * file = luaL_checkstring(L, -1) ;
	int len = read_file(file, read_buf, buf_len);
	if (len== 0) {
		//raise_error(L, "[proto error]: fail to read proto_desc file=%s\n!", file);
		//Log(DefaultLogFile, "engine", ERR, "read file %s error!", file);
		return PROTO_ERROR;
	}
	int status = luaL_loadbuffer(L, read_buf, len, 0);
	if (0 == status && lua_isfunction(L, -1)) {
		lua_newtable(L); //置空加载环境，确保安全加载
		if (lua_setfenv(L, -2) != 1) {
			lua_settop(L, top);
			return PROTO_ERROR;
		}
		int status = lua_pcall(L, 0, 1, 0);
		if (status) {
			//Log(DefaultLogFile, "engine", ERR, "lua call error!");
			lua_settop(L, top);
			return PROTO_ERROR;
		}
		return convert_protocol(L);
	} else {
		//raise_error(L, "[proto error]: invalid proto_desc file=%s!\n", file);
		//lua_settop(L, top);
		//并不平衡栈，而是把出错的字符串返回给上层，
		//不怕栈泄露，因为一旦出错，引擎就退出了。
		lua_settop(L, top + 1);
		return PROTO_ERROR;
	}
}

int net_protocol::process_type(lua_State *L, const char* path)
{
	std::string name = path;
	size_t begin_pos = name.find_last_of('/');
	if (begin_pos < 0) {
		begin_pos = name.find_last_of('\\');
	}
	size_t end_pos = name.find_last_of('.');
	if ((begin_pos < 0) || (end_pos < 0) || (begin_pos >= end_pos)){
		//raise_error (L, "[proto error]: invalid protocol path,\n<%s>!\n", path);
		return PROTO_ERROR;
	}
	++begin_pos; // 去掉"/"
	_pack_func_name = name.substr(begin_pos, end_pos-begin_pos);
	if (_pack_func_name.substr(0, 2) == "c_") {
		_is_maker = (cur_char == client);
	}
	else if (_pack_func_name.substr(0, 2) == "s_") {
		_is_maker = (cur_char == server);
	}
	else {
		//raise_error (L, "[proto error]: invalid protocol path! \n<%s>!\n", path);
		return PROTO_ERROR;
	}
	return PROTO_OK;
}

int net_protocol::convert_protocol(lua_State*L)
{
	int top = lua_gettop(L);
	// 栈顶必须为协议文件加载后返回的table
	if (!lua_istable(L, -1)) {
		//Log(DefaultLogFile, "engine", ERR, "top is not table");
		return PROTO_ERROR;
	}
	// 解析函数名称
	lua_pushstring(L, func_name);
	lua_rawget(L, -2);
	if (lua_isstring(L, -1)) {
		_pack_func_name = lua_tostring(L, -1);
		//Log(DefaultLogFile, "engine", ERR, "syn proto funcname ok!");
	}
	lua_pop(L, 1);

	// 解析协议类型
	int rel = func_proc(L);
	if (rel != PROTO_OK) {
		//Log(DefaultLogFile, "engine", ERR, "syn proto type error!");
		lua_settop(L, top);
		return rel;
	}
	// 解析参数列表
	rel = arglist_proc(L);
	if (rel != PROTO_OK) {
		//Log(DefaultLogFile, "engine", ERR, "syn proto arg error!");
		lua_settop(L, top);
		return rel;
	}
	lua_settop(L, top);
	return PROTO_OK;
}

int net_protocol::func_proc(lua_State* L)
{
	int top = lua_gettop(L);
	proto_manager* ProtoManager = this->GetProtoManager();
	if (_is_maker) {
		// 当前为远程函数实现者
		lua_pushstring(L, ProtoManager->GetForMaker());
				lua_rawget(L, LUA_GLOBALSINDEX);
		if (!lua_istable(L, -1)) { // 用于存需要实现的远程函数，供其它终端远程调用
			//这个太多了，暂时先不记log，但是下面的不绑定是一定要记的
			//Log(DefaultLogFile, "engine", WAR, "[proto warnning]: can't find maker's func table!");
			//return proto_error;
			goto use_tmpfunc;
		}
		lua_pushstring(L, _pack_func_name.c_str());
		lua_rawget(L, -2);
		if (!lua_isfunction(L, -1)) {
			//Log(DefaultLogFile, "engine", WAR, "[proto warnning]: can't find far_func funcname=%s!", _pack_func_name.c_str());
			goto use_tmpfunc;
		}
		
		
		
		if (_unpack_ref != LUA_NOREF) {
			lua_unref(L, _unpack_ref);
		}
		_unpack_ref = lua_ref(L, true);
		assert(_unpack_ref != LUA_NOREF);
		lua_settop(L, top);
		return (_unpack_ref != LUA_NOREF) ? PROTO_OK:PROTO_ERROR;
		
		use_tmpfunc:
		lua_getglobal(L, "construc_default_rpcfunc");
		if (!lua_isfunction(L, -1)) {
			//Log(DefaultLogFile, "engine", ERR, "[proto error]: can't find construc_default_rpcfunc!");
		}
		else {
			lua_pushstring(L, _pack_func_name.c_str());
			lua_pcall(L, 1, 1, 0);
			if (_unpack_ref != LUA_NOREF) {
				lua_unref(L, _unpack_ref);
			}
			assert(lua_isfunction(L, -1));
			//Log(DefaultLogFile, "engine", WAR, "[proto warnning]: %s bind to default_rpc_func!", _pack_func_name.c_str());
			_unpack_ref = lua_ref(L, true);
			assert(_unpack_ref != LUA_NOREF);
			lua_settop(L, top);
		}
		return (_unpack_ref != LUA_NOREF) ? PROTO_OK : PROTO_ERROR;
//return proto_error;
/*
		if (!lua_istable(L, -1)) { // 用于存需要实现的远程函数，供其它终端远程调用
			//raise_error(L, "[proto error]: can't find maker's funcs!");
			return PROTO_ERROR;
		}
		lua_pushstring(L, _pack_func_name.c_str());
		lua_rawget(L, -2);
		if (!lua_isfunction(L, -1)) {
			//raise_error(L, "[proto error]:can't find far_func:%s", _pack_func_name.c_str());
			return PROTO_ERROR;
		}
		if (_unpack_ref != LUA_NOREF) {
			lua_unref(L, _unpack_ref);
		}
		_unpack_ref = lua_ref(L, true);
		assert(_unpack_ref != LUA_NOREF);
		lua_settop(L, top);
		return (_unpack_ref != LUA_NOREF) ? PROTO_OK : PROTO_ERROR;
*/
	}
	else {
		// 当前为远程函数调用者
		lua_pushstring(L, ProtoManager->GetForCaller());
		lua_rawget(L, LUA_GLOBALSINDEX);
		if (!lua_istable(L, -1)) { 
			// 用于存放供远程调用的函数
			lua_newtable(L);
			lua_setglobal(L, ProtoManager->GetForCaller());
			lua_pushstring(L, ProtoManager->GetForCaller());
			lua_rawget(L, LUA_GLOBALSINDEX);
			assert(lua_istable(L, -1));
		}
		// 用upvalue将协议格式数据绑定到打包函数
		lua_pushstring(L, _pack_func_name.c_str());
		lua_pushlightuserdata(L, this);
		lua_pushcclosure(L, pack, 1);
		lua_rawset(L, -3);
		lua_settop(L, top);
		return PROTO_OK;
	}
	return PROTO_ERROR;
}

int net_protocol::arglist_proc(lua_State* L)
{
	// 栈顶为记录协议格式的table
	lua_pushstring(L, arg_list);
	lua_rawget(L, -2);
	if (lua_istable(L, -1)) {
		int arg_list_i = lua_gettop(L);
		for (int i=1; i<=static_cast<int>(lua_objlen(L, arg_list_i)); ++i) {
			lua_settop(L, arg_list_i);
			lua_rawgeti(L,arg_list_i, i);		// 获取参数描述信息
			if (!lua_istable(L, -1)) {
				//raise_error(L, "[proto error]: invalid arg_list desc!");
				return PROTO_ERROR;
			}
			int desc_i = lua_gettop(L);
			lua_rawgeti(L, desc_i, 2);	// 获取参数类型
			std::string arg_type = luaL_checkstring(L, -1);
			lua_rawgeti(L, desc_i, 3);
			fcall_base_arg* arg = 0;
			if (lua_isnumber(L, -1)) {	// 是否为数组
				// 数组型参数
				int array_len = static_cast<int>(lua_tonumber(L, -1));
				if (array_len < 0 || array_len > 255) {
					//raise_error(L, "[proto error]: array error:%s!", arg_type.c_str());
					return PROTO_ERROR;
				}
				arg = new fcall_array_arg(arg_type.c_str(), array_len);
				assert(arg);
				fcall_arg_manager::add_arg(arg);
			}
			else {
				arg = fcall_arg_manager::get_arg(arg_type.c_str());
			}
			if (arg) {
				_args.push_back(arg);
			}
			else {
				//raise_error(L, "[proto error]: read invalid arg_list!");
				return PROTO_ERROR;
			}
		}
		return PROTO_OK;
	}
	return PROTO_ERROR;
}

int net_protocol::marshal(lua_State * L, byte * buf, size_t buf_len)
{
	int used_len = 0 ; 
	// 先填入协议id 
	assert(this->_id != 0); // 编号0用于标记协议号长度
	if (this->_id < 256) {
		memcpy(buf, &this->_id, 1);
		used_len = 1 ;
	}
	else {
		*buf = 0;
		memcpy(buf+1, &this->_id, 2);
		used_len = 3 ;
	}

	// 参数打包
	for (int i=0; i<(int)this->_args.size(); ++i) {
		assert(this->_args[i]);
		// i + 2 表示lua stack 里的参数的位置
		int cur_used = this->_args[i]->pack(L, i+2, buf+used_len, buf_len-used_len);
		if (cur_used == 0) {
			//raise_error (L,"pack argument fail. protocol_id:%d, nth_arg:%d, expect_type:%s\n",
											//this->get_id(), i, this->_args[i]->get_type());
			return -1;
		}
		used_len += cur_used;
	}

	return used_len ;
}

// 需注册到虚拟机，故必须为静态函数
// 因为现在都是在脚本里直接打包，若pack 出错，就error到脚本层 , 脚本会handle 这个error.
// 当在脚本里 for_caller.func(vfd, ...) 的时候，就调用到这个函数。
// 当//raise_error发生的话，可能是网络或cpu负载过重, 这样可能会加剧严重并引起雪崩。
// 但为了引起重视和调试方便，还是用//raise_error不用_RUNTIME_ERROR吧。
int net_protocol::pack(lua_State* L)
{
	//每次pack都尝试 FlushOutSockBuf
	//FlushOutSockBuf();
	static const VFD  VFD_NULL = 0 ;
	int top = lua_gettop(L);
	net_protocol *self = static_cast<net_protocol*>(lua_touserdata(L, lua_upvalueindex(1)));
	//不管是否正确pack都进行统计
	net_protocol::_total_pack_count++ ;
	self->_pack_count++ ;
	
	//替换mbuf中的缓冲区,因为现在不需要添加以前的控制头
	byte buf[MAXTONETD];
	proto_manager* ProtoManager = self->GetProtoManager();


	//读取附加参数(即服务器端远程函数的第一个参数)
	const int type = lua_type (L, 1) ;
	if (type == LUA_TNUMBER) {
		VFD ext = static_cast<int>(lua_tonumber(L, 1));
		if  (VFD_NULL >= ext) {
			//虚拟登陆时，这类log太多太多了，先屏蔽掉 by wulf
			//_WARN("drop unicast to VFD_NULL ptoid=%d,VFD_NULL=%d", self->get_id(), VFD_NULL) ;
			goto outway ;
		}
		// 获取缓存
		if (buf == NULL) {
			goto outway ;
		}
		int used_len = self->marshal (L, buf, MAXTONETD) ;
		if (used_len > 0) {
			ProtoManager->GetSendHookFunc()(buf, used_len, ext, 0, 0);
		} 
		else {
				//Log(DefaultLogFile, "engine", ERR, "gamer pack data error,id=%d,name=%s", self->get_id(), (self->_pack_func_name).c_str());
		}
	} else if (type == LUA_TTABLE) {
		//byte data[MAXTONETD] ;为了减少一次memcpy，决定直接从OutSockBuf里直接分配一块大的内存
		const size_t s = lua_objlen (L, 1) ;
		if (s > 0) {
			//分配最大的数据包的内存
			//byte * data = (byte*)MbufAllocPack(OutSockBuf, MAXTONETD) ;
			if (buf == NULL) {
				//	self->get_id(), MAXTONETD) ;
				goto outway ;
			}
			int used_len = self->marshal (L, buf, MAXTONETD) ;
			if (used_len > 0) {
				size_t c = 0 , t = 0 ;
				byte * beginp = &buf[used_len] ; 
				byte * endp = &buf[MAXTONETD] ; 
				for (size_t i = 0 ; i < s ; ++i) {
					lua_pushnumber (L, i + 1) ;
					lua_gettable(L, 1) ;
					const VFD to = (const VFD)luaL_checknumber (L, -1) ;
					lua_remove(L, -1) ;
					if (to > VFD_NULL) {
						t++ ;
						if (beginp+sizeof(VFD) <= endp) {
							*(VFD*)beginp = to ;
							beginp += sizeof(VFD) ;
							c++ ;
						}
					}
				}
				if (c > 0) {
					size_t total = used_len + c * sizeof(VFD) ;
					ProtoManager->GetSendHookFunc()(buf, total, c, 1, used_len);
					//Log(DefaultLogFile, "engine", DBG, "Send to Game Server, pto id=%d,name=%s", self->get_id(), (self->_pack_func_name).c_str());
					if (c < t) { //只广播一部分vfd.数据已经发送了，然后才raise出通知信息
						//TODO
					}
				} 
				else {//c == 0
					//TODO
				}
			} 
			else { //used_len
				//TODO
			}
		} // lua_objlen
	} 
	else { //type == 
		//TODO:
	}


outway :

	lua_settop(L, top);
	return 0;
}

// 要区分是解包出错还是脚本逻辑处理正确的数据包出错。
int net_protocol::unpack(lua_State*L, const byte* buf, int buf_len, int ext)
{
	int top = lua_gettop(L);
	int readed_len = 0;
	//协议不一致的话
	//assert(_unpack_ref != LUA_NOREF);
	//DUMP_STAT_BEGIN 
	if (_unpack_ref == LUA_NOREF) {
		//Log(DefaultLogFile, "engine", ERR, "no unpack function ,id:%d", _id);
		return XYNET_ERROR_UNPACK_FORMAT ;
	}
	net_protocol::_total_unpack_count++ ;
	_unpack_count++ ;
	//VfdMbuf.SetLastUnpackTime (ext) ;

	lua_getref(L, _unpack_ref);
	assert(lua_isfunction(L, -1));
	// 压入服务器端远程函数的默认参数
	lua_pushnumber(L, ext);
	


	// 其它远程参数压栈
	for (size_t i=0; i<_args.size(); ++i) {
		assert(_args[i]);
	
		int cur_readed = _args[i]->unpack(L, buf+readed_len, buf_len-readed_len);
		if (cur_readed == 0) {
			//Log(DefaultLogFile, "engine", ERR, "unpack argument fail. id:%d, seq:%d, type:%s",_id, i, _args[i]->get_type());
			lua_settop(L, top);
			//解包时数据包格式出错
			return XYNET_ERROR_UNPACK_FORMAT ;
		}
		readed_len += cur_readed;
	}

	//DUMP_STAT_END("unpack id=%d,count=%d", _id, _unpack_count) 

	 // 调用远程函数,参数数量为 默认参数加远程参数数量
	int status ;  
	//DUMP_STAT_BEGIN 
	status = lua_pcall(L, static_cast<int>(_args.size()+1), 0, 0);
	//DUMP_STAT_END("PtoCall id=%d,count=%d", _id, _unpack_count) 
	
	if (status) {
		const char* traceback = luaL_checkstring(L, -1);
		//Log(DefaultLogFile, "engine", ERR, "Gamer unpack data,run lua function error:%s", traceback);
		lua_settop(L, top);
		//处理数据包逻辑出错
		return XYNET_ERROR_UNPACK_SCRIPT ;
	}
	else {
		lua_settop(L, top);
		return readed_len; //必定大于 0
	}
}

int net_protocol::get_check_id(void)
{
	std::string buf(_pack_func_name); 
	for (size_t i=0; i<_args.size(); ++i) {
		buf += _args[i]->get_type();
		buf += _args[i]->get_hashstr();
	}
	//printf("the pto hash str:%s %d\n",buf.c_str(),buf.length());
	//return calc_hashnr(buf.c_str(), buf.length());
	int result = calc_hashnr(buf.c_str(), buf.length()); 
	//printf("the compute result:%d\n", result);
	return result;
}

//////////////////////////////////////////////////////////////////////////
// 协议管理器
//std::vector<net_protocol*> proto_manager::_s_protos ;
//send_hook_t proto_manager::s_data_sender = NULL;
//unsigned proto_manager::_static_protocol_count = 0 ;

proto_manager::proto_manager(send_hook_t func):_for_maker(NULL),_for_caller(NULL),_static_protocol_count(0)
{
	s_data_sender = func;
	fcall_arg_manager::init();
	// 添加一个默认协议，占用编号0
	if (_s_protos.size() == 0) {
		_s_protos.push_back(new net_protocol(0, this));
	}
}

proto_manager::~proto_manager()
{
	for (size_t i = 0; i<_s_protos.size(); ++i) {
		if (_s_protos[i]) {
			delete _s_protos[i];
		}
	}
	_s_protos.clear();
	fcall_arg_manager::destruct();
}

//在脚本层输出统计信息
int proto_manager::stat (lua_State * L)
{
	lua_newtable (L) ;
	lua_pushstring (L, "total_pack_count") ;
	lua_pushinteger (L, net_protocol::_total_pack_count) ;
	lua_settable (L, -3) ;
	for (size_t i = 0; i<_s_protos.size(); ++i) {
		lua_pushinteger (L, _s_protos[i]->get_id()) ;
		lua_pushinteger (L, _s_protos[i]->_pack_count) ;
		lua_settable (L, -3) ;
	}

	lua_newtable (L) ;
	lua_pushstring (L, "total_unpack_count") ;
	lua_pushinteger (L, net_protocol::_total_unpack_count) ;
	lua_settable (L, -3) ;
	for (size_t i = 0; i<_s_protos.size(); ++i) {
		lua_pushinteger (L, _s_protos[i]->get_id()) ;
		lua_pushinteger (L, _s_protos[i]->_unpack_count) ;
		lua_settable (L, -3) ;
	}

	return 2 ;
}

int proto_manager::add_arg_type(lua_State* L)
{
	fcall_arg_manager::add_table_args(L);
	return 0;
}

// 在这里处理预留的协议号
int proto_manager::add_static_protocol (lua_State *L)
{
	int ret = add_protocol(L) ;
	if (1 == ret) 
		_static_protocol_count++ ;

	return ret ;
}

int proto_manager::add_protocol(lua_State* L)
{
	// 依照协议添加顺序自动分配协议ID
	assert(_s_protos.size() > 0); // 不可使用协议号0

	unsigned int proto_id = _s_protos.size();
	net_protocol* proto = new net_protocol(proto_id, this);
	if (proto->load(L) == PROTO_OK) {
		_s_protos.push_back(proto);
		lua_pushnumber(L, proto_id);
		return 1;
	}
	else {
		lua_pushstring(L, "load protocol failed!") ;
		lua_pushboolean(L, 0) ;
		//raise_error(L, "[proto_manager]: load protocol failed!");
		return 2;
	}
}

int proto_manager::update_protocol(lua_State* L)
{
	int top = lua_gettop(L);
	if ((top >= 2) && lua_isstring(L, -1) && lua_isnumber(L, -2)) {
		unsigned int proto_id = (unsigned int)lua_tonumber(L, -2);
		if (proto_id>=0 && proto_id<_s_protos.size()) {
			assert(_s_protos[proto_id]);
			net_protocol* proto = new net_protocol(proto_id, this);
			if (proto->load(L) == PROTO_OK) {
				delete _s_protos[proto_id];
				_s_protos[proto_id] = proto;
				lua_pushnumber(L, proto_id);
				return 1;
			}
			else {
				delete proto;
				lua_pushstring(L, "update protocol failed!") ;
				lua_pushboolean(L, 0) ;
				//raise_error(L, "[proto_manager]: update protocol failed!");
				return 2;
			}
		}
	}
	return 0;
}

// 传入需解析的数据，返回实际解析的长度
// 返回值必须大于 0
int proto_manager::unpack_data(lua_State*L, const byte* buf, int buf_size, int ext)
{
	try
	{
		// 解析协议编号
		int readed_len = 1;
		check_buf(readed_len, buf_size);
		unsigned int proto_id = 0;
		memcpy(&proto_id, buf, 1);

		if (proto_id == 0) {
			proto_id = 0;
			readed_len += 2;
			check_buf(readed_len, buf_size);
			memcpy(&proto_id, buf+1, 2);
		}

		std::vector<net_protocol *> _s_protos = GetProtos();
		if (proto_id >= _s_protos.size()) {
			return XYNET_ERROR_UNPACK_FORMAT ;
		}

		net_protocol* proto = _s_protos[proto_id];
		
		if (proto) {
			//Log(DefaultLogFile, "engine", DBG, "Get Game server pto Name:%s", (proto->GetPackFuncName()).c_str()); 
			int cur_readed = proto->unpack(L, buf+readed_len, buf_size-readed_len, ext);
			if (cur_readed < 0)  //出错
				return cur_readed ;
			if (cur_readed == 0 && (proto->GetProtoArgs()).size () > 0) {
				return XYNET_ERROR_UNPACK_FORMAT ;
			}
			readed_len += cur_readed; 
			return readed_len;
		}
		else {
			//Log(DefaultLogFile, "engine", ERR, "Gamer unpack data error:pto name:%s", (proto->GetPackFuncName()).c_str());
			return XYNET_ERROR_UNPACK_FORMAT ;
		}
	}
	catch (...)
	{
		//Log(DefaultLogFile, "engine", ERR, "Gamer unpack data error,throw exception");
		return XYNET_ERROR_UNPACK_FORMAT ;
	}
}

int proto_manager::get_ptos_checkid(lua_State*L)
{
	int* buf = new int[_s_protos.size()];
	for (size_t i=0; i<_s_protos.size(); ++i) {
		buf[i] = (_s_protos[i])?_s_protos[i]->get_check_id():0;
	}
	lua_pushnumber(L, calc_hashnr((const char*)buf, sizeof(int)*_s_protos.size()));
	delete [] buf;
	return 1;
}

void proto_manager::SetForMaker(char* ForMaker)
{
	_for_maker = (char *)malloc(strlen(ForMaker)+1);	
	strncpy(_for_maker, ForMaker, strlen(ForMaker)+1);
}

void proto_manager::SetForCaller(char* ForCaller)
{
	_for_caller = (char *)malloc(strlen(ForCaller)+1);	
	strncpy(_for_caller, ForCaller, strlen(ForCaller)+1);
}

int GetProtoObj(lua_State* L)
{
	char* KeyName = (char *)luaL_checkstring(L, 1); 
	string KeyStr(KeyName);
	if(!ProtoManagerMap.count(KeyStr)) return 0;

	proto_manager* pobj = ProtoManagerMap[KeyStr];
	lua_pushlightuserdata(L, (void*)pobj);

	luaL_getmetatable(L, "meta.pto.object");
	lua_setmetatable(L, -2);
	return 1;
}

int pto_add_arg_type(lua_State* L)
{
	proto_manager* ProtoManager = (proto_manager*)luaL_checkudata(L, 1, "meta.pto.object");	
	return ProtoManager->add_arg_type(L);
}

int pto_add_protocol(lua_State* L)
{
	proto_manager* ProtoManager = (proto_manager*)luaL_checkudata(L, 1, "meta.pto.object");	
	return ProtoManager->add_protocol(L);
}

int pto_update_protocol(lua_State* L)
{
	proto_manager* ProtoManager = (proto_manager*)luaL_checkudata(L, 1, "meta.pto.object");	
	return ProtoManager->update_protocol(L);
}

int pto_add_static_protocol(lua_State* L)
{
	proto_manager* ProtoManager = (proto_manager*)luaL_checkudata(L, 1, "meta.pto.object");	
	return ProtoManager->add_static_protocol(L);
}

int pto_get_ptos_checkid(lua_State* L)
{
	proto_manager* ProtoManager = (proto_manager*)luaL_checkudata(L, 1, "meta.pto.object");	
	return ProtoManager->get_ptos_checkid(L);
}

int pto_stat(lua_State* L)
{
	proto_manager* ProtoManager = (proto_manager*)luaL_checkudata(L, 1, "meta.pto.object");	
	return ProtoManager->stat(L);
}


static const struct luaL_Reg ProtoLib_f[] ={
	{"GetProtoObj", GetProtoObj},
	{NULL, NULL}
};

static const struct luaL_Reg ProtoLib_m[] = {
	{ "add_arg_type", pto_add_arg_type},
    { "add_protocol", pto_add_protocol},
    { "update_protocol", pto_update_protocol},
    { "add_static_protocol", pto_add_static_protocol},
    { "get_check_sum", pto_get_ptos_checkid},
    { "stat", pto_stat},
    { NULL, NULL}
};

bool InitProtocolLib(lua_State * L) {
    luaL_newmetatable(L, "meta.pto.object");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, ProtoLib_m);
    luaL_register(L, "pto.object", ProtoLib_f);
	return true;
}

proto_manager* CreateNewProtoManager(string ProtoName, char* ForMaker, char* ForCaller,send_hook_t func)
{
	if(ProtoManagerMap.count(ProtoName)) return NULL;

	proto_manager* pobj = new proto_manager(func);
	pobj->SetForMaker(ForMaker);
	pobj->SetForCaller(ForCaller);

	ProtoManagerMap[ProtoName] = pobj;
	return pobj;
}

proto_manager* GetProtoManager(string ProtoName)
{
	if(!ProtoManagerMap.count(ProtoName)) return NULL;
	return ProtoManagerMap[ProtoName];
}
