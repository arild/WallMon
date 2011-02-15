include ../Makefile.inc

EXECUTABLE_NAME=wallmond
SRC=*.cpp

all: wallmond

wallmond: $(SRC)
	$(CC) $(CFLAGS_COMMON) -o $(EXECUTABLE_NAME) $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_COMMON)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)

