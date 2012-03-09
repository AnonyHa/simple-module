LIBEVENT_PATH=3rd/libevent-1.4.12-stable
LIBEVENT_INC=$(LIBEVENT_PATH)
LIBEVENT_LIB=$(LIBEVENT_PATH)/.libs/libevent.a

OBJ=packet.o


All:
	g++ -c client_socket.cpp -I $(LIBEVENT_INC)
	g++ -o test client_socket.o $(LIBEVENT_LIB) $(OBJ)
	g++ -o server_test server_test.cpp $(LIBEVENT_LIB) -I $(LIBEVENT_INC)
clean:
	rm -rf *.o test
