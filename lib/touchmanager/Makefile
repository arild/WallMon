include ../../Makefile.inc

all: TouchManager
	
clean:
	rm -f *.o
	rm -f *.gch

TouchManager: STouchManager.h STouchManager.cpp WVGeometry.h WVGeometry.cpp
	$(CC) -O2 -c -fPIC -Wno-multichar $(INC_PATH) $(LIB_PATH) $?
	ar rcs lib$@.a STouchManager.o WVGeometry.o
	mv *.a ..