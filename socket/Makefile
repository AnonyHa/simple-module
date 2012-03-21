LIBEVENT_PATH=3rd/libevent-1.4.12-stable
LIBEVENT_INC=$(LIBEVENT_PATH)
LIBEVENT_LIB=$(LIBEVENT_PATH)/.libs/libevent.a
INC_PATH=./include

OBJ=packet.o

All:
	g++ -c peer_point.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -c server_socket.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -c server.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -c socket_manager.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -c client_socket.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -c packet_interface.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
#	g++ -o server socket_manager.o client_socket.o server_socket.o peer_point.o packet_interface.o server.o $(LIBEVENT_LIB)
	g++ -c client.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
#	g++ -o client socket_manager.o server_socket.o client_socket.o peer_point.o packet_interface.o client.o $(LIBEVENT_LIB)
clean:
	rm -rf *.o server client
