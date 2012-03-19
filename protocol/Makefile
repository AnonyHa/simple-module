LUA_PATH=./3rd/lua-5.1.4
LUA_INC=$(LUA_PATH)/src
LUA_LIB=$(LUA_PATH)/src/liblua.a
INC_PATH=./include

All:
	g++ -c data_format.cpp -I $(LUA_INC) -I $(INC_PATH)
	g++ -c protocol.cpp -I $(LUA_INC) -I $(INC_PATH)
	g++ -c test.cpp -I $(LUA_INC) -I $(INC_PATH)
	g++ -o test test.o protocol.o data_format.o $(LUA_LIB)
clean:
	rm -rf *.o test
