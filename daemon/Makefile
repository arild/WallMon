include ../Makefile.inc

EXECUTABLE_NAME=wallmond
SRC=*.cpp *.h
CFLAGS= -Wno-multichar -rdynamic

all: wallmond

wallmond: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_COMMON)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)

