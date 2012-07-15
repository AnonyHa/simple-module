All:
	g++ -c PacketProcess.cpp #-D__UNIT_TEST__
	g++ -c mybuf.cpp
	g++ -o test PacketProcess.o mybuf.o
clean:
	rm -rf test *.o
