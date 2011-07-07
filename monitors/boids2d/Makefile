include ../Makefile.inc
include ../../Makefile.inc

SRC = $(shell find . -name \*.cpp)
OBJS = $(SRC:.cpp=.o)

# Take display wall into account on Rocksvv cluster
HOSTNAME = $(shell hostname)
ifeq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
ROCKSVV += -D ROCKSVV
endif

LDFLAGS=-lm -lConfig -lwSystem -lTouchManager -lWallView -lpthread -lboost_thread -lglog -lfreetype -lz -lftgl `sdl-config --libs`
ifeq ($(PLATFORM), Darwin)	
	LDFLAGS+=-framework OpenGL
	OBJS+=WMAutorelease.o
else
	LDFLAGS+=-lGL
endif
LDFLAGS+= -lProcessCollector -lshout

libboids2d.so: $(OBJS)
	$(CC) -D $(PLATFORM) -o $@ $(CFLAGS_SO_CREATE) $(LIB_PATH) $^ $(LDFLAGS)
	mv *.so ../bin

all: libboids2d.so
			
clean:
	rm -f *.o
	rm -f *.gch
	rm -f *.so

%.o:%.cpp
	$(CC) -D $(PLATFORM) -Wno-multichar $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) -c -o $@ $<

%.o:%.mm
	$(CC) -D $(PLATFORM) -Wno-multichar -framework Cocoa $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) -c -o $@ $<
        
depend: .depend
.depend:
	$(CC) -g -Wno-multichar $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) -MM $(SRC) > $@

-include .depend
        