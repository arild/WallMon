include ../Makefile.inc

CFLAGS = -c -fPIC
DIRS_PRE = stubs 
DIRS_POST = touchmanager

%.o:%.cpp
	$(CC) -o $@ -c -fPIC $(INC_PATH) $<

all: SubDirsPre current SubDirsPost
	
current: SubDirsPre ClassLoader System Config PracticalSocket ByteBuffer LinuxProcessMonitor LinuxProcessMonitorLight IoLogger WallView PidMonitor MonitorDispatcher ProcessCollector

SubDirsPre: 
	@ for dir in ${DIRS_PRE}; do (cd $${dir}; ${MAKE}) ; done

SubDirsPost: SubDirsPre current
	@ for dir in ${DIRS_POST}; do (cd $${dir}; ${MAKE}) ; done

clean: 	
	@ for dir in ${DIRS_PRE}; do (cd $${dir}; ${MAKE} clean) ; done
	${CLEAN}
	@ for dir in ${DIRS_POST}; do (cd $${dir}; ${MAKE} clean) ; done

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
MonitorDispatcher: MonitorDispatcher.o stubs/MonitorDispatcher.pb.o PracticalSocket.o ClassLoader.o
	${MAKE_STATIC_LIB}

# Note that the protobuf stub (ProcessCollector.pb.o) is included
# This library is build as a shared library due to some problems
# with protobuf and static linking
ProcessCollector: ProcessCollector.o LinuxProcessMonitorLight.o PidMonitor.o stubs/ProcessCollector.pb.o
	${MAKE_SHARED_LIB}
			
include ../Makefile.dep.inc
