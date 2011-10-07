include ../../Makefile.inc

LIBS=-lLinuxProcessMonitor -lSystem -lProcessCollector
# Do not compile with graphics on rocksvv 
LIB_PLPLOT=-lplplotcxxd -lplplotd -lltdl -ldl -lm -lcsirocsa -lqhull -lqsastime -lfreetype
HOSTNAME = $(shell hostname)
ifneq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
	GRAPHICS = -D GRAPHICS
	LIBS += $(LIB_PLPLOT)
endif

all: libchartmon.so
			
clean:
	${CLEAN}
	
%.o:%.cpp
	$(CC) -o $@ $(GRAPHICS) $(CFLAGS_SO_PREPARE) $(INC_PATH) $<

libchartmon.so: $(OBJS)
	$(CC) -o $@ $(CFLAGS_SO_CREATE) $(LIB_PATH) $^ $(LIBS)
	mv *.so ../bin

include ../../Makefile.dep.inc