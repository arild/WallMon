include ../Makefile.inc

CFLAGS = -c -fPIC

%.o:%.cpp
	$(CC) -o $@ -c -fPIC $(INC_PATH) $<

all: stubs_
	make all_
	
all_: touchmanager_ ClassLoader System Config PracticalSocket ByteBuffer LinuxProcessMonitor LinuxProcessMonitorLight IoLogger WallView PidMonitor MonitorDispatcher ProcessCollector
	
clean: 	
	${CLEAN}
	$(MAKE) clean -C stubs
	$(MAKE) clean -C touchmanager
	
stubs_:
	$(MAKE) -C stubs
	
touchmanager_:
	$(MAKE) -C touchmanager

System: System.o 
	${MAKE_STATIC_LIB}
	
Config: Config.o
	${MAKE_STATIC_LIB}

PracticalSocket: PracticalSocket.o
	${MAKE_STATIC_LIB}

ClassLoader: ClassLoader.o
	${MAKE_STATIC_LIB}

ByteBuffer: ByteBuffer.o
	${MAKE_STATIC_LIB} 

LinuxProcessMonitor: LinuxProcessMonitor.o
	${MAKE_STATIC_LIB}

LinuxProcessMonitorLight: LinuxProcessMonitorLight.o 
	${MAKE_STATIC_LIB}

IoLogger: IoLogger.o
	${MAKE_STATIC_LIB}

WallView: WallView.o
	${MAKE_STATIC_LIB}

PidMonitor: PidMonitor.o
	${MAKE_STATIC_LIB}

# Note that the protobuf stub (MonitorDispatcher.pb.o) is included
MonitorDispatcher: MonitorDispatcher.o PracticalSocket.o ClassLoader.o
	${MAKE_STATIC_LIB} PracticalSocket.o ClassLoader.o stubs/MonitorDispatcher.pb.o

# Note that the protobuf stub (ProcessCollector.pb.o) is included
# This library is build as a shared library due to some problems
# with protobuf and static linking
ProcessCollector: ProcessCollector.o LinuxProcessMonitorLight.o PidMonitor.o
	${MAKE_SHARED_LIB} LinuxProcessMonitorLight.o PidMonitor.o stubs/ProcessCollector.pb.o
			
include ../Makefile.dep.inc
