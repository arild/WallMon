include ../../Makefile.inc

LIBs=-lSystem -lProcessCollector

all: gnuplot.so
			
%.o:%.cpp
	$(CC) -o $@ $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) $<

gnuplot.so: $(OBJS)
	$(CC) -o $@ $(CFLAGS_SO_CREATE) $(LIB_PATH) $^ $(LIBS)
	mv *.so ../bin
	
clean:
	${CLEAN}

