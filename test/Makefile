include ../Makefile.inc

LIBS=-lpthread -lgtest -lboost_thread -lboost_system -ldl -lev -lglog -lprotobuf -lPracticalSocket -lPs -lLinuxProcessMonitorLight -lClassLoader -lByteBuffer -lConfig -lSystem -lWallmonMessage -lIoLogger -lMonitorDispatcher -lWallView -lPidMonitor

all: test

test: $(OBJS)
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS)

clean:
	${CLEAN} ; rm -f test

include ../Makefile.dep.inc