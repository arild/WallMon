USER = $(shell whoami)
include /home/$(USER)/WallMon/Makefile.inc
 SRC=Main.cpp
LIB=-lwSystem -lLinuxProcessMonitor -lpthread

all: thread_tester

clean:
	rm -f *.o
	rm -f thread_tester

thread_tester: $(SRC)
	$(CC) $(INC_PATH) $(LIB_PATH) $(SRC) $(LIB) -o $@
