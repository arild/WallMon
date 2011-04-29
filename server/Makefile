include ../Makefile.inc

LIB_LOCAL=-lByteBuffer -lMonitorDispatcher
EXECUTABLE_NAME=wallmons
SRC=*.cpp *.h

all: wallmons

wallmons: $(SRC)
	$(CC) $(CFLAGS_COMMON) -o $(EXECUTABLE_NAME) $(SRC) $(INC_PATH) $(LIB_PATH) -L/home/local/lib $(LIB_COMMON) $(LIB_LOCAL)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)

