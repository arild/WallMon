include ../Makefile.inc
include ../../Makefile.inc

LIB_GRAPHIC=-lGL -lfreetype -lz -lftgl -lSDL
LIB=-lm -lConfig -lSystem -lTouchManager -Wl,-Bstatic -lshout -Wl,-Bdynamic -lWallView -lProcessCollector -lpthread -lboost_thread -lglog $(LIB_GRAPHIC)
SRC=*.cpp *.h

# Do not compile with graphics on rocksvv 
HOSTNAME = $(shell hostname)
ifeq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
ROCKSVV += -D ROCKSVV
endif

USER = $(shell whoami)
all: $(SRC)
	$(CC) -g -Wno-multichar $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) -I/home/$(USER)/include/freetype2 $(SRC)
	$(CC) -g -Wno-multichar $(CFLAGS_SO_CREATE) -L/home/local/lib $(LIB_PATH) -o libboids2d.so *.o $(LIB)
	mv *.so ../bin
			
clean:
	rm -f *.o
	rm -f *.so

