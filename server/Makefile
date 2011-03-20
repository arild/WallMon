include ../Makefile.inc

#LIB_PLPLOT=-lplplotcxxd -lplplotd -lltdl -ldl -lm -lcsirocsa  -lqhull -lqsastime -lfreetype
LIB_LOCAL=-lboost_filesystem -lByteBuffer
EXECUTABLE_NAME=wallmons
SRC=*.cpp *.h

all: wallmons

wallmons: $(SRC)
	$(CC) $(CFLAGS_COMMON) -o $(EXECUTABLE_NAME) $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_COMMON) $(LIB_LOCAL) #`pkg-config --cflags --libs plplotd-c++`

clean:
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)

