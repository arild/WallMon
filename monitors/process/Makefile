include ../Makefile.inc
include ../../Makefile.inc

LIB=-lpthread -lglog -lprotobuf
SRC=*.cpp

all: $(SRC)
	$(CC) $(CFLAGS_SO_PREPARE) $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB)
	$(CC) $(CFLAGS_SO_CREATE) -o libprocessmon.so  *.o
	$(INSTALL_CMD)
		
clean:
	rm -f *.o
	rm -f *.so

