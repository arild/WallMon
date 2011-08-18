include ../Makefile.inc

LIBS=-lpthread -lgtest -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lByteBuffer -lConfig -lSystem -lWallmonMessage -lIoLogger -lMonitorDispatcher -lWallView -lPidMonitor

all: test

test: $(OBJS)
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

clean:
	${CLEAN} ; rm -f test

include ../Makefile.dep.inc