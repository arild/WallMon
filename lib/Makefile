include ../Makefile.inc

CFLAGS = -O2 -c -fPIC
DIRS_PRE = stubs 
DIRS_POST = touchmanager

all: SubDirsPre ClassLoader System Config PracticalSocket ByteBuffer LinuxProcessMonitor IoLogger WallView PidMonitor MonitorDispatcher ProcessCollector SubDirsPost
	
clean: 	
	@ for dir in ${DIRS_PRE}; do (cd $${dir}; ${MAKE} clean) ; done
	rm -f *.o
	rm -f *.a
	@ for dir in ${DIRS_POST}; do (cd $${dir}; ${MAKE} clean) ; done
	
SubDirsPre:
	@ for dir in ${DIRS_PRE}; do (cd $${dir}; ${MAKE}) ; done

System: System.h System.cpp 
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

Config: Config.h Config.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

PracticalSocket: PracticalSocket.h PracticalSocket.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

ClassLoader: ClassLoader.h ClassLoader.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

ByteBuffer: ByteBuffer.h ByteBuffer.cpp 
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

LinuxProcessMonitor: LinuxProcessMonitor.h LinuxProcessMonitor.cpp 
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar rcs lib$@.a $@.o

IoLogger: IoLogger.h IoLogger.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

WallView: WallView.h WallView.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

PidMonitor: PidMonitor.h PidMonitor.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o

# Note that the protobuf stub (MonitorDispatcher.pb.o) is included
MonitorDispatcher: PracticalSocket.o ClassLoader.o MonitorDispatcher.h MonitorDispatcher.cpp IMonitorManager.h
	$(CC) $(CFLAGS) $(INC_PATH) $?
	ar cru lib$@.a $@.o stubs/MonitorDispatcher.pb.o

# Note that the protobuf stub (ProcessCollector.pb.o) is included
OBJS=LinuxProcessMonitor.o PidMonitor.o stubs/ProcessCollector.pb.o
ProcessCollector: $(OBJS) ProcessCollector.h ProcessCollector.cpp
	$(CC) $(CFLAGS_SO_PREPARE) $(INC_PATH) $?
	$(CC) $(CFLAGS_SO_CREATE) $(LIB_PATH) -o lib$@.so $(OBJS) ProcessCollector.o $(LIB)
	mv lib$@.so $(SYSTEM)/lib # Should not be necessary, but otherwise it is not found...
		
SubDirsPost:
	@ for dir in ${DIRS_POST}; do (cd $${dir}; ${MAKE}) ; done
	