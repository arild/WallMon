include ../../Makefile.inc

%.o:%.cpp
	$(CC) -o $@ -c -fPIC $(INC_PATH) $<

all: STouchManager
	
clean:
	${CLEAN}
	
STouchManager: STouchManager.o WVGeometry.o
	${MAKE_STATIC_LIB}
	mv *.a ..
	
include ../../Makefile.dep.inc