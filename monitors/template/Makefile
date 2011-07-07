include ../Makefile.inc
include ../../Makefile.inc

SRC=*.cpp *.h

all: test.so

test.so: $(SRC)
	$(CC) $(CFLAGS_SO_PREPARE) $(INC_PATH) $(SRC)
	$(CC) $(CFLAGS_SO_CREATE) $(LIB_PATH) -o $@ *.o
	mv $@ ../bin
			
clean:
	rm -f *.o
	rm -f *.so

