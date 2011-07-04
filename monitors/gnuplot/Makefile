include ../Makefile.inc
include ../../Makefile.inc

LIB=-lwSystem -lProcessCollector
SRC=*.cpp *.h

all: $(SRC)
	protoc --cpp_out=. *.proto
	$(CC) -g $(CFLAGS_SO_PREPARE) $(INC_PATH) $(SRC)
	$(CC) -g $(CFLAGS_SO_CREATE) $(LIB_PATH) -o gnuplot.so *.o $(LIB)
	cp *.so ../bin
			
clean:
	rm -f *.o
	rm -f *.so
	rm -f *.gch

