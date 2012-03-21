INC_PATH=./include

All:
	g++ -g -c mybuf.cpp -I $(INC_PATH)
	g++ -g -c packet_process.cpp -I $(INC_PATH)
	g++ -g -c test.cpp -I $(INC_PATH)
#	g++ -g -o test test.o packet_process.o mybuf.o


clean:
	rm -rf *.o test test.core
