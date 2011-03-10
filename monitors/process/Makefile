include ../Makefile.inc
include ../../Makefile.inc

LIB=-lpthread -lglog -lprotobuf -lLinuxProcessMonitor
SRC=*.cpp
CFLAGS_LOCAL=-O2

# Do not compile with graphics on rocksvv 
LIB_PLPLOT=-lplplotcxxd -lplplotd -lltdl -ldl -lm -lcsirocsa -lqhull -lqsastime -lfreetype
HOSTNAME = $(shell hostname)
ifneq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
	CFLAGS_LOCAL += -D GRAPHICS
	LIB += $(LIB_PLPLOT)
endif

all: $(SRC)
	$(CC) $(CFLAGS_LOCAL) $(CFLAGS_SO_PREPARE) $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB) 
	$(CC) $(CFLAGS_SO_CREATE) -o libprocessmon.so  *.o
	$(INSTALL_CMD)
		
clean:
	rm -f *.o
	rm -f *.so

