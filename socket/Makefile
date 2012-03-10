LIBEVENT_PATH=3rd/libevent-1.4.12-stable
LIBEVENT_INC=$(LIBEVENT_PATH)
LIBEVENT_LIB=$(LIBEVENT_PATH)/.libs/libevent.a
INC_PATH=./include

OBJ=packet.o

All:
	#g++ -c client_socket.cpp -I $(LIBEVENT_INC)
	g++ -c peer_point.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -c server_socket.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -c server.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -o server server.o server_socket.o peer_point.o $(LIBEVENT_LIB)
	g++ -c client_socket.cpp -I $(LIBEVENT_INC) -I $(INC_PATH)
	g++ -o client client_socket.o peer_point.o $(LIBEVENT_LIB)
	#g++ -o test client_socket.o $(LIBEVENT_LIB) $(OBJ)
	#g++ -o server_test server_test.cpp $(LIBEVENT_LIB) -I $(LIBEVENT_INC)
clean:
	rm -rf *.o server client
