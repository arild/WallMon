include ../Makefile.inc

CFLAGS = -O2 -c $(INC_PATH) $(LIB_PATH) -lglog
DIRS = 	processcollector

all: PracticalSocket ClassLoader System ByteBuffer LinuxProcessMonitor WallmonMessage IoLogger
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE}) ; done

clean: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE} clean) ; done
	rm -f *.o
	rm -f *.a

PracticalSocket: 
	$(CC) $(CFLAGS) PracticalSocket.cpp
	ar cru libPracticalSocket.a PracticalSocket.o

ClassLoader: 
	$(CC) $(CFLAGS) ClassLoader.cpp
	ar cru libClassLoader.a ClassLoader.o

System: 
	$(CC) $(CFLAGS) System.cpp
	ar cru libSystem.a System.o

ByteBuffer: 
	$(CC) $(CFLAGS) ByteBuffer.cpp
	ar cru libByteBuffer.a ByteBuffer.o

LinuxProcessMonitor: 
	$(CC) -c LinuxProcessMonitor.cpp
	ar rcs libLinuxProcessMonitor.a LinuxProcessMonitor.o

WallmonMessage:
	protoc --cpp_out=. *.proto 
	$(CC) $(CFLAGS) -c -lprotobuf Wallmon.pb.cc
	ar rcs libWallmonMessage.a Wallmon.pb.o

IoLogger:
	$(CC) $(CFLAGS) -c IoLogger.cpp
	ar cru libIoLogger.a IoLogger.o


