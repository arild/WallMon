include ../Makefile.inc

LIB_LOCAL=-lboost_filesystem
EXECUTABLE_NAME=wallmons
SRC=*.cpp

all: wallmons

wallmons: $(SRC)
	$(CC) $(CFLAGS_COMMON) -o $(EXECUTABLE_NAME) $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_COMMON) $(LIB_LOCAL)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)

