include ../Makefile.inc

CFLAGS=-rdynamic
LIBS= -lMonitorDispatcherMessage -lMonitorDispatcher -lConfig -lpthread -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lSystem -lWallmonMessage -lIoLogger

all: wallmond

clean:
	${CLEAN}
	rm -f wallmond

wallmond: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(INC_PATH) $(LIB_PATH) $^ $(LIBS)

include ../Makefile.dep.inc