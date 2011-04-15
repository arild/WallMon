include ../Makefile.inc
include ../../Makefile.inc

LIB_GRAPHIC=-lGLU -lGL -lfreetype -lz -lftgl -lglut -lSDL
LIB=-lSystem -lProcessCollector -lpthread -lboost_thread -lglog $(LIB_GRAPHIC)
SRC=*.cpp *.h

# Do not compile with graphics on rocksvv 
HOSTNAME = $(shell hostname)
ifneq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
	GRAPHICS += -D GRAPHICS
endif

all: $(SRC)
	$(CC) $(GRAPHICS) $(CFLAGS_SO_PREPARE) $(INC_PATH) -I/home/arild/include/freetype2 $(SRC)
	$(CC) $(CFLAGS_SO_CREATE) $(LIB_PATH) -o libboids2d.so *.o $(LIB)
	cp *.so ../bin
			
clean:
	rm -f *.o
	rm -f *.so

