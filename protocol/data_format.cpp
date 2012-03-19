
// $Id: data_format.cpp 83932 2009-06-02 03:11:31Z mcj $
#include "data_format.h"
#include "stdio.h"
#include "lbacktrace.h"
#include "string.h"
#include "log.h"
/*
void dump_error (lua_State *L, const char *fmt, ...)
{
	va_list var ;
        va_start (var, fmt) ;
	vfprintf (stderr, fmt, var) ;
       	va_end (var) ;
   
        if (lua_isstring(L, -1)) {
                  fprintf(stderr, "%s\n", luaL_checkstring(L, -1)) ;
        }
 }
*/

long file_size(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

int read_file(const char* fpath, char* buf, int buf_len)
{	
	FILE* f = fopen(fpath, "r");
	if (f) {
		int len = file_size(f);
		if (len > buf_len) {
			return 0;
		}
		len = static_cast<int>(fread(buf, 1, len, f));
		fclose(f);
		return len;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 数字型参数
int fcall_number_arg::pack(lua_State*L, int data_in, byte* buf, int buf_len)
{
	int num;
	unsigned int data;
	number_type data_t;
	int used_len = 0;
	luaL_checktype(L, data_in, LUA_TNUMBER);
	lua_number2int(num, lua_tonumber(L, data_in));
	if (num < 0) { // 处理数字的符号
		data = -num;
		data_t.is_negative = true;
	}
	else {
		data = num;
		data_t.is_negative = false;
	}
	// 决定传输数字所用buf长度
	if (data <= 0xff) {
		data_t.len = 1;
		used_len = (sizeof(data_t) + 1);
	}
	else if (data<= 0xffff) {
		data_t.len = 2;
		used_len = (sizeof(data_t) + 2);
	}
	else if (data<= 0xffffff) {
		data_t.len = 3;
		used_len = (sizeof(data_t) + 3);
	}
	else if (data<= 0xffffffff) {
		data_t.len = 4;
		used_len = (sizeof(data_t) + 4);
	}
	else {
		// number最大支持0xffffffff;
		data_t.len = 0;
		//raise_error(L, "[pack error]: accpept invalid number!");
		return 0;
	}
	check_buf(used_len, buf_len);
	memcpy(buf, &data_t, sizeof(data_t));
	memcpy(buf+sizeof(data_t), &data, data_t.len);
	return used_len;
}

int fcall_number_arg::unpack(lua_State*L, const byte* buf, int buf_len)
{
	number_type data_t;
	int readed_len = 1;
	check_buf(readed_len, buf_len);
	memcpy(&data_t, buf, sizeof(data_t));
	if ((data_t.len <= 0) || (data_t.len >= 5)) {
		Log(DefaultLogFile, "engine", ERR, "[unpack error]: invalid number data");
		return 0;
	}

	int data = 0;
	readed_len += data_t.len;
	check_buf(readed_len, buf_len);
	memcpy(&data, buf+sizeof(data_t), data_t.len);
	if (data_t.is_negative) {
		data = -data;
	}
	lua_pushnumber(L, data);
	return readed_len;
}

//////////////////////////////////////////////////////////////////////////
// 单字节数字型参数
int fcall_byte_arg::pack(lua_State*L, int data_in, byte* buf, int buf_len)
{
	int used_len = 1;
	int num;
	luaL_checktype(L, data_in, LUA_TNUMBER);
	lua_number2int(num, lua_tonumber(L, data_in));
	check_buf(used_len, buf_len);
	memcpy(buf, &num, sizeof(char));
	return used_len;
}

int fcall_byte_arg::unpack(lua_State*L, const byte* buf, int buf_len)
{
	int readed_len = 1;
	check_buf(readed_len, buf_len);
	lua_pushnumber(L, *static_cast<const byte*>(buf));
	return readed_len;
}

//////////////////////////////////////////////////////////////////////////
// 双字节数字型参数
int fcall_word_arg::pack(lua_State*L, int data_in, byte* buf, int buf_len)
{
	int used_len = 2;
	int num;
	luaL_checktype(L, data_in, LUA_TNUMBER);
	lua_number2int(num, lua_tonumber(L, data_in));
	check_buf(used_len, buf_len);
	memcpy(buf, &num, 2);
	return used_len;
}

int fcall_word_arg::unpack(lua_State*L, const byte* buf, int buf_len)
{
	int readed_len = 2;
	check_buf(readed_len, buf_len);
	lua_pushnumber(L, *(const short*)buf);
	return readed_len;
}

//////////////////////////////////////////////////////////////////////////
// 四字节数字型参数
int fcall_dword_arg::pack(lua_State*L, int data_in, byte* buf, int buf_len)
{
	int used_len = 4;
	int num;
	luaL_checktype(L, data_in, LUA_TNUMBER);
	lua_number2int(num, lua_tonumber(L, data_in));
	check_buf(used_len, buf_len);
	memcpy(buf, &num, sizeof(num));
	return used_len;
}

int fcall_dword_arg::unpack(lua_State*L, const byte* buf, int buf_len)
{
	int readed_len = 4;
	check_buf(readed_len, buf_len);
	lua_pushnumber(L, *(const int*)buf);
	return readed_len;
}

int fcall_string_arg::pack(lua_State*L, int data_in, byte* buf, int buf_len)
{
	int used_len = 0;
	size_t str_len;
	const char* data = luaL_checklstring(L, data_in, &str_len);
	int str_id = static_cast<int>(str_len) + 1;
	if (str_id <= 0xff) {
	// string 长度小于256
		used_len = static_cast<int>(str_len+1); // str_lenò??ó1
		check_buf(used_len, buf_len);
		memcpy(buf, &str_id, 1);
		memcpy(buf+1, data, str_len);
		return used_len;
	}
	else if (str_id <= 0xffff ) {
		// string  长度大于256
		used_len = static_cast<int>(str_len+3);// str_lenò??ó1
		check_buf(used_len, buf_len);
		memset(buf, 0, 1); // (str_id==0)表示string长度采用随后两个字节表示
		memcpy(buf+1, &str_id, 2);
		memcpy(buf+3, data, str_len);
		return used_len;
	}
	else {
		// string最大长度支持0xffff;
		//raise_error(L, "[pack error]: accpept invalid string!");
		return 0;
	}
}

int fcall_string_arg::unpack(lua_State*L, const byte* buf, int buf_len)
{
	int readed_len = 1;
	check_buf(readed_len, buf_len);
	int str_id = static_cast<int>(*static_cast<const byte*>(buf));
	if (str_id > 0) {
		int str_len = str_id - 1;
		if (str_len == 0) {
			lua_pushstring(L, "");
		}
		else {
			readed_len += str_len;
			check_buf(readed_len, buf_len);
			lua_pushlstring(L, (const char*)(buf+1), str_len);
		}
	}
	else {
		assert(str_id == 0);
		readed_len = 3;
		check_buf(readed_len, buf_len);
		memcpy(&str_id, buf+1, 2);
		int str_len = str_id - 1;
		if (str_len < 0 ) {//zero len is ok.
			Log(DefaultLogFile, "engine", ERR, "unpack string length error:str_len=%d", str_len);
			return 0;
		}
		readed_len += str_len;
		check_buf(readed_len, buf_len);
		lua_pushlstring(L, (const char*)(buf+3), str_len);
	}
	return readed_len;
}


//////////////////////////////////////////////////////////////////////////
// 复合嵌套式数据

fcall_table_arg::table_opcode::table_opcode(tb_op_code op_code, int ref, fcall_base_arg* arg)
{
	op = op_code;
	key_ref = ref;
	value_arg = arg;
}

fcall_table_arg::fcall_table_arg(lua_State*L)
{
	if (_read_format(L) != PROTO_OK) {
		//raise_error(L, "[init table error]: load table format failed!");
	}
}

// 加载复杂数据的格式描述信息
int fcall_table_arg::_read_format(lua_State*L)
{
	int top = lua_gettop(L);
	const int buf_len = 100 * 1024;
	char read_buf[buf_len];
	int len = read_file(luaL_checkstring(L, -1), read_buf, buf_len);
	if (len== 0) {
		//raise_error(L, "[init table error]: read table_desc file failed!");
		return PROTO_ERROR;
	}
	int status = luaL_loadbuffer(L, read_buf, len, 0);
	if (!status && lua_isfunction(L, -1)) {
		lua_newtable(L); //置空加载环境，确保安全加载
		if (lua_setfenv(L, -2) != 1) {
			lua_settop(L, top);
			return PROTO_ERROR;
		}
		int status = lua_btcall(L, 0, 2, 0);
		if (status) {
			lua_settop(L, top);
			return PROTO_ERROR;
		}
		_type = luaL_checkstring(L, -2);
		_hashstr = luaL_checkstring(L, -2);
		_cur_table_deep = 0;
		if (lua_istable(L, -1)) {
			_travel_table(L, -1); // 遍历样本table，构建op_code序列
			//_opcodes.pop_back();  // 忽略最后一个table结束操作
		}
		else {
			lua_settop(L, top);
			//raise_error(L, "[init table error]: invalid table format data!");
			return PROTO_ERROR;
		}
		lua_settop(L, top);
		return PROTO_OK;
	}
	lua_settop(L, top);
	return PROTO_ERROR;
}

void fcall_table_arg::_travel_table(lua_State*L, int table_i)
{
	int top = lua_gettop(L);
	// 防止递归导致的栈溢出
	if (++_cur_table_deep > max_table_deep) {
		lua_settop(L, top);
		//raise_error(L, "table is too deep!") ;
		return;
	}
	if (table_i < 0) {
		//将栈内数据的相对索引转为绝对索引
		int top = lua_gettop(L);
		assert(top >= -table_i);
		table_i = top + 1 + table_i;
	}

	fcall_base_arg* value_arg = 0;
	int key_ref = LUA_NOREF;
	// 遍历指定table构建opcode序列
	///* table is in the stack at index 't' */
	lua_pushnil(L);  /* first key */
	while (lua_next(L, table_i) != 0) {
		value_arg = 0;
		key_ref = LUA_NOREF;
		// key
		if (lua_isnumber(L, -2)) {
			_hashstr += "number";
			lua_pushvalue(L, -2);
			key_ref = lua_ref(L, true); // 保存key的ref，提高解析速度
			assert(key_ref != LUA_NOREF);
		}
		else if (lua_isstring(L, -2)) {
			_hashstr += luaL_checkstring(L, -2);
			lua_pushvalue(L, -2);
			key_ref = lua_ref(L, true); // 保存key的ref，提高解析速度
			assert(key_ref != LUA_NOREF);
		}
		else {
			lua_unref(L, key_ref);
			//raise_error(L, "[travl_table error]: read invalid key!");
		}

		// value
		if (lua_isnumber(L, -1)) {
			_hashstr += "number";
			value_arg = fcall_arg_manager::get_arg("number");
			if (!value_arg) {
				lua_unref(L, key_ref);
				//raise_error(L, "[travl_table error]: invalid table value type!");
			}
			else {
				assert(key_ref != LUA_NOREF);
				_opcodes.push_back(table_opcode(push_value, key_ref, value_arg));
			}
		}
		else if (lua_isstring(L, -1)) {
			_hashstr += luaL_checkstring(L, -1);
			value_arg = fcall_arg_manager::get_arg(luaL_checkstring(L, -1));
			if (!value_arg) {
				lua_unref(L, key_ref);
				//raise_error(L, "[travl_table error]: invalid table value type!");
			}
			else {
				assert(key_ref != LUA_NOREF);
				_opcodes.push_back(table_opcode(push_value, key_ref, value_arg));
			}
		}
		else if (lua_istable(L, -1)) {
			_opcodes.push_back(table_opcode(tb_begin, key_ref, 0));
			_travel_table(L, -1); // 遍历子table
		} 
		else {
			lua_unref(L, key_ref);
			//raise_error(L, "[travl_table error]: read invalid value!");
		}
		lua_pop(L, 1);  /* removes 'value'; keeps 'key' for next iteration */
	}
	_opcodes.push_back(table_opcode(tb_end, key_ref, value_arg));
	lua_settop(L, top);
}

int fcall_table_arg::pack(lua_State*L, int data_in, byte* buf, int buf_len)
{
	int top = lua_gettop(L);
	int used_len = 0;
	if (lua_istable(L, data_in)) {
		lua_pushvalue(L, data_in);
		// 忽略第一条tb_begin操作
		for (size_t i=0; i<_opcodes.size(); ++i) { 
			table_opcode& opcode = _opcodes[i];
			if (opcode.op == tb_begin) {
				// 创建子table
				assert(opcode.key_ref > 0);
				lua_getref(L, opcode.key_ref);	// push key
				lua_rawget(L, -2);
				if (!lua_istable(L, -1)) {
					lua_settop(L, top);
					//print_error(L, "[pack error]: invalid source table!");
					luaL_typerror(L, -1, "table") ;
					return 0;
				}
			}
			else if (opcode.op == push_value) {	
				// 打包数据
				assert(opcode.key_ref != LUA_NOREF);
				lua_getref(L, opcode.key_ref);	// push key
				lua_rawget(L, -2);
				if (!opcode.value_arg) {
					lua_settop(L, top);
					//raise_error(L, "[pack error]: invalid table value_arg!");
					return 0;
				}
				int cur_used = opcode.value_arg->pack(L, lua_gettop(L), buf+used_len, buf_len-used_len);
				if (cur_used == 0) {
					lua_settop(L, top);
					return 0;
				}
				used_len += cur_used;
				lua_pop(L, 1);
			}
			else if (opcode.op == tb_end) {
				// 弹出当前子table
				lua_pop(L, 1); 
			}
			else {
				lua_settop(L, top);
				//raise_error(L, "[pack error]: invalid table opcode!");
				return 0;
			}
		}
	}
	else {
		//print_error(L, "[pack error]: the target packed must be a table!");
		return luaL_typerror(L, data_in, "table") ;
	}
	lua_settop(L, top);
	return used_len;
}

int fcall_table_arg::unpack(lua_State*L, const byte* buf, int buf_len)
{
	int top = lua_gettop(L);
	int readed_len = 0;
	lua_newtable(L); // 创建欲还原的table
	for (size_t i=0; i<_opcodes.size()-1; ++i) { // 忽略最后一个tb_end
		table_opcode& opcode = _opcodes[i];
		if (opcode.op == tb_begin) {
			assert(opcode.key_ref != LUA_NOREF);
			lua_getref(L, opcode.key_ref);	// push key
			lua_newtable(L);
		}
		else if (opcode.op == push_value) {	// value只可能是num或str
			assert((opcode.key_ref != LUA_NOREF) && opcode.value_arg);
			lua_getref(L, opcode.key_ref);	// push key
			int cur_readed = opcode.value_arg->unpack(L, buf+readed_len, buf_len-readed_len);
			if (cur_readed == 0) {
				lua_settop(L, top);
				return 0;
			}
			readed_len += cur_readed;
			lua_settable(L, -3);
		}
		else if (opcode.op == tb_end) {
			// 完成当前子table的添加			
			lua_settable(L, -3);
		}
		else {
			//print_error(L, "[unpack error]: invalid table opcode!");
			lua_settop(L, top);
			return readed_len;
		}
	}
	return readed_len;
}


//////////////////////////////////////////////////////////////////////////
// 数组型参数
fcall_array_arg::fcall_array_arg(const char* base_type, int size)
{
	assert(base_type);
	_type = "array";
	_hashstr = "array";
	_hashstr += base_type;
	_base_packer = fcall_arg_manager::get_arg(base_type);
	_hashstr += _base_packer->get_hashstr();
	_size = size;
	assert(_base_packer);
}

int fcall_array_arg::pack(lua_State*L, int data_in, byte* buf, int buf_len)
{
	int top = lua_gettop(L);
	if (lua_istable(L, data_in)) {
		int used_len = 0;
		int array_len = _size;
		if (array_len == 0) {
			array_len = static_cast<int>(lua_objlen(L, data_in));
			used_len = 1;
			check_buf(used_len, buf_len);
			*buf = static_cast<byte>(array_len);
		}
		if (array_len > 512) {
			// 数组最大长度为255
			//raise_error(L, "[pack error]: array is too big!");
			return 0;
		}
		assert(_base_packer);
		for (int i=1; i<= array_len; ++i) {
			// 遍历table数组中的元素
			lua_rawgeti(L, data_in, i);
			int cur_used = _base_packer->pack(L, -1, buf+used_len, buf_len-used_len);
			if (cur_used == 0) {
				lua_settop(L, top);
				return 0;
			}
			used_len += cur_used;
			lua_pop(L, 1) ;
		}
		return used_len;
	}
	else {
		//print_error(L, "[pack error]: accpept invalid array, must be a valid table!");
		//return PROTO_OK;
		return luaL_typerror(L, data_in, "table") ;
	}
}

int fcall_array_arg::unpack(lua_State*L, const byte* buf, int buf_len)
{
	int top = lua_gettop(L);
	lua_newtable(L); // 创建数组
	int data_i = top + 1;
	assert(_base_packer);
	int array_len = _size;
	int readed_len = 0;
	if (array_len == 0) {
		readed_len = 1;
		check_buf(readed_len, buf_len);
		memcpy(&array_len, buf, 1);
	}
	// assert(array_len > 0);
	for (int i=0; i<array_len; ++i) {
		lua_pushnumber(L, i+1);
		int cur_readed = _base_packer->unpack(L, buf+readed_len, buf_len-readed_len);
		if (cur_readed == 0) {
			lua_settop(L, top);
			return 0;
		}
		readed_len += cur_readed;
		lua_rawset(L, data_i);
	}
	assert(lua_gettop(L) == data_i);
	//lua_settop(L, data_i);
	return readed_len;
}

//////////////////////////////////////////////////////////////////////////
// 数据解析管理器
std::vector<fcall_base_arg*> fcall_arg_manager::_s_args;
void fcall_arg_manager::init()
{
	// number、dword和string为默认基础类型
	_s_args.push_back(new fcall_number_arg);
	_s_args.push_back(new fcall_byte_arg);
	_s_args.push_back(new fcall_word_arg);
	_s_args.push_back(new fcall_dword_arg);
	_s_args.push_back(new fcall_string_arg);
}

void fcall_arg_manager::destruct()
{
	for (size_t i=0; i<_s_args.size(); ++i) {
		if (_s_args[i]) {
			delete _s_args[i];
		}
	}
	_s_args.clear();
}

void fcall_arg_manager::add_table_args(lua_State*L)
{
	// 栈顶为table格式描述文件路径
	_s_args.push_back(new fcall_table_arg(L));
}

fcall_base_arg* fcall_arg_manager::get_arg(int arg_id)
{
	if (arg_id >=0 && arg_id < static_cast<int>(_s_args.size())) {
		return _s_args[arg_id];
	}
	return 0;
}

fcall_base_arg* fcall_arg_manager::get_arg(const char* type_name)
{
	for (size_t i=0; i<_s_args.size(); ++i) {
		if (_s_args[i]->check_type(type_name)) {
			return _s_args[i];
		}
	}
	return 0;
}
