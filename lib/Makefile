include ../Makefile.inc

CFLAGS = -O2 -c $(INC_PATH) $(LIB_PATH) -lglog

all: PracticalSocket.a ClassLoader.a System.a ByteBuffer

PracticalSocket.a: PracticalSocket.o
	ar cru libPracticalSocket.a PracticalSocket.o

PracticalSocket.o: PracticalSocket.cpp
	$(CC) $(CFLAGS) PracticalSocket.cpp

ClassLoader.a: ClassLoader.o
	ar cru libClassLoader.a ClassLoader.o

ClassLoader.o: ClassLoader.cpp
	$(CC) $(CFLAGS) ClassLoader.cpp

System.a: System.o
	ar cru libSystem.a System.o

System.o: System.cpp
	$(CC) $(CFLAGS) System.cpp

ByteBuffer: 
	$(CC) $(CFLAGS) ByteBuffer.cpp
	ar cru libByteBuffer.a ByteBuffer.o
	
clean:
	rm -f *.o
	rm -f *.a

