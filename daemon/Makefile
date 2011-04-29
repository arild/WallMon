include ../Makefile.inc

EXECUTABLE_NAME=wallmond
SRC=*.cpp *.h

all: wallmond

wallmond: $(SRC)
	$(CC) $(CFLAGS_COMMON) $(INC_PATH) $(LIB_PATH) $(SRC) $(LIB_COMMON) -o $@

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)

