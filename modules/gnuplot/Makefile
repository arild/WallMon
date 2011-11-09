include ../../Makefile.inc

LIBS=-lSystem -lProcessCollector -lWallView

all: libgnuplot.so
			
clean:
	${CLEAN}

%.o:%.cpp
	$(CC) -o $@ $(CFLAGS_SO_PREPARE) $(INC_PATH) $<

libgnuplot.so: $(OBJS)
	$(CC) -o $@ $(CFLAGS_SO_CREATE) $(LIB_PATH) $^ $(LIBS)
	mv *.so ../bin
	
include ../../Makefile.dep.inc
