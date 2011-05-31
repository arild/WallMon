include ../../Makefile.inc

all: TouchManager
	
clean:
	rm -f *.o

TouchManager: STouchManager.h STouchManager.cpp WVGeometry.h WVGeometry.cpp
	$(CC) -O2 -c -fPIC -Wno-multichar $(INC_PATH) $(LIB_PATH) $? -lpthread -lshout
	ar rcs lib$@.a STouchManager.o WVGeometry.o
	mv *.a ..