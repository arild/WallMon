include ../Makefile.inc

CFLAGS = -O2 -c
DIRS = stubs 

all: SubDirs PracticalSocket ClassLoader System ByteBuffer LinuxProcessMonitor IoLogger WallView MonitorDispatcher ProcessCollector
	
clean: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE} clean) ; done
	rm -f *.o
	rm -f *.a
	
SubDirs:
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE}) ; done

PracticalSocket: PracticalSocket.h PracticalSocket.cpp
	$(CC) $(CFLAGS) $?
	ar cru lib$@.a $@.o

ClassLoader: ClassLoader.h ClassLoader.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

System: System.h System.cpp 
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

ByteBuffer: ByteBuffer.h ByteBuffer.cpp 
	$(CC) $(CFLAGS) $?
	ar cru lib$@.a $@.o

LinuxProcessMonitor: LinuxProcessMonitor.h LinuxProcessMonitor.cpp 
	$(CC) -c $?
	ar rcs lib$@.a $@.o

IoLogger: IoLogger.h IoLogger.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

WallView: WallView.h WallView.cpp
	$(CC) $(CFLAGS) $?
	ar cru lib$@.a $@.o

# Note that the protobuf stub (MonitorDispatcher.pb.o) is included
MonitorDispatcher: MonitorDispatcher.h MonitorDispatcher.cpp IMonitorManager.h
	$(CC) $(CFLAGS) $(INC_PATH) $(LIB_PATH) -lPracticalSocket -lClassLoader $?
	ar cru libMonitorDispatcher.a $@.o stubs/MonitorDispatcher.pb.o

# Note that the protobuf stub (ProcessCollector.pb.o) is included
ProcessCollector: ProcessCollector.h ProcessCollector.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar rcs lib$@.a $@.o LinuxProcessMonitor.o System.o stubs/ProcessCollector.pb.o
