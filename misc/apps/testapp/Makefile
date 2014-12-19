
include ~/WallMon/Makefile.inc
SRC=TestApp.cpp
LIB=-lwSystem -lWallView -lLinuxProcessMonitor -lglog

all: TestApp

clean:
	rm -f *.o
	rm -f TestApp

TestApp: $(SRC)
	$(CC) -o $@ $(INC_PATH) $(LIB_PATH) $(SRC) $(LIB)
