LIBEVENT_PATH=./socket/3rd/libevent-1.4.12-stable
LIBEVENT_INC=$(LIBEVENT_PATH)
LIBEVENT_LIB=$(LIBEVENT_PATH)/.libs/libevent.a

SOCKET_OBJ=socket/peer_point.o socket/server_socket.o socket/socket_manager.o socket/client_socket.o socket/packet_interface.o
ENCRYPT_OBJ=encrypt/mybuf.o encrypt/packet_process.o

INC_PATH=-I ./socket/include -I ./encrypt/include -I ./protocol/include -I $(LIBEVENT_PATH)

All:
	g++ -c server.cpp $(INC_PATH)
	g++ -o server server.o $(SOCKET_OBJ) $(ENCRYPT_OBJ) $(LIBEVENT_LIB)
	g++ -c client.cpp $(INC_PATH)
	g++ -o client client.o $(SOCKET_OBJ) $(ENCRYPT_OBJ) $(LIBEVENT_LIB)
clean:
	rm -rf *.o server client
