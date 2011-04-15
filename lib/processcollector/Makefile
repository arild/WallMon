include ../../Makefile.inc

CFLAGS=-O2 -c
LIB=-lLinuxProcessMonitor -lSystem
SRC=*.cpp *.cc *.h

all: $(SRC)
	protoc --cpp_out=. *.proto
	$(CC) $(CFLAGS) $(INC_PATH) $(SRC)
	ar rcs libProcessCollector.a *.o ../LinuxProcessMonitor.o ../System.o
	mv *.a ..

clean:
	rm -f *.o

