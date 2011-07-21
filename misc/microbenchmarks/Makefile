USER = $(shell whoami)
include /home/$(USER)/WallMon/Makefile.inc

SRC=*.cpp
LIB=-lprotobuf -lwSystem -lLinuxProcessMonitorLight -lProcessCollectorMessage -lprofiler -lglog

all: benchmark

clean:
	${CLEAN}
	rm -f benchmark

benchmark: $(SRC)
	g++ -o $@ -O2 $(INC_PATH) $(LIB_PATH) $(SRC) $(LIB)
