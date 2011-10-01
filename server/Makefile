include ../Makefile.inc

CFLAGS= -rdynamic
LIBS=-lpthread -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lByteBuffer -lConfig -lSystem -lWallmonMessage -lIoLogger -lMonitorDispatcher `sdl-config --libs`

all: wallmons

wallmons: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(INC_PATH) $(LIB_PATH) $(LIBS)

clean:
	${CLEAN}
	rm -f wallmons

include ../Makefile.dep.inc
