

include ../Makefile.inc

LIB_LOCAL=-lpthread -lboost_thread -lgtest -lConfig -lByteBuffer -lSystem -lWallView -lglog -lPidMonitor
EXECUTABLE_NAME=test
SRC=*.cpp

all: test

test: $(SRC)
	$(CC) $(CFLAGS_COMMON) -o $(EXECUTABLE_NAME) $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_LOCAL)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)