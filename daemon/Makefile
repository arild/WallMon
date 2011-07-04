include ../Makefile.inc

EXECUTABLE_NAME=wallmond
SRC=*.cpp *.h

all: wallmond

wallmond: $(SRC)
	$(CC) $(CFLAGS_COMMON) -o $@ $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_COMMON)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)

