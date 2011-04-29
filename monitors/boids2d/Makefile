include ../Makefile.inc
include ../../Makefile.inc

LIB_GRAPHIC=-lGLU -lGL -lfreetype -lz -lftgl -lglut -lSDL
LIB=-lSystem -lWallView -lProcessCollector -lpthread -lboost_thread -lglog $(LIB_GRAPHIC)
SRC=*.cpp *.h

# Do not compile with graphics on rocksvv 
HOSTNAME = $(shell hostname)
ifeq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
ROCKSVV += -D ROCKSVV
endif

all: $(SRC)
	$(CC) -g $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) -I/home/arild/include/freetype2 $(SRC)
	$(CC) -g $(CFLAGS_SO_CREATE) -L/home/local/lib $(LIB_PATH) -o libboids2d.so *.o $(LIB)
	mv *.so ../bin
			
clean:
	rm -f *.o
	rm -f *.so

