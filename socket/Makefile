LIBEVENT_PATH=3rd/libevent-1.4.12-stable
LIBEVENT_INC=$(LIBEVENT_PATH)
LIBEVENT_LIB=$(LIBEVENT_PATH)/.libs/libevent.a


All:
	g++ -o client_socket.o client_socket.cpp -I $(LIBEVENT_INC)
	g++ -o test client_socket.o $(LIBEVENT_LIB)
clean:
	rm -rf *.o test
