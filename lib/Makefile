include ../Makefile.inc

CFLAGS = -O2 -c $(INC_PATH) $(LIB_PATH) -lglog

all: PracticalSocket ClassLoader System ByteBuffer LinuxProcessMonitor

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


#LinuxProcessMonitor:
#	$(CC) -static -shared -c -fPIC LinuxProcessMonitor.cpp
#	$(CC) -static -lc -shared -o libLinuxProcessMonitor.so LinuxProcessMonitor.o

clean:
	rm -f *.o
	rm -f *.a

