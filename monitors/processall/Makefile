include ../Makefile.inc
include ../../Makefile.inc

LIB=-lLinuxProcessMonitor -lSystem
SRC=*.cpp

# Do not compile with graphics on rocksvv 
LIB_PLPLOT=-lplplotcxxd -lplplotd -lltdl -ldl -lm -lcsirocsa -lqhull -lqsastime -lfreetype
HOSTNAME = $(shell hostname)
ifneq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
	GRAPHICS += -D GRAPHICS
	LIB += $(LIB_PLPLOT)
endif

all: $(SRC)
	$(CC) $(GRAPHICS) $(CFLAGS_SO_PREPARE) $(INC_PATH) $(SRC)
	$(CC) $(CFLAGS_SO_CREATE) $(LIB_PATH) -o libprocessallmon.so *.o $(LIB)
	cp *.so ../bin
			
clean:
	rm -f *.o
	rm -f *.so

