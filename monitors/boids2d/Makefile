include ../Makefile.inc
include ../../Makefile.inc

SRC = $(shell find . -name \*.cpp)
OBJS = $(SRC:.cpp=.o)

# Take display wall into account on Rocksvv cluster
HOSTNAME = $(shell hostname)
ifeq ("$(HOSTNAME)", "rocksvv.cs.uit.no")
ROCKSVV += -D ROCKSVV
endif

LDFLAGS=-lm -lConfig -lSystem -lTouchManager -lWallView -lpthread -lboost_thread -lglog -lfreetype -lz -lftgl `sdl-config --libs`
ifeq ($(PLATFORM), Darwin)	
	LDFLAGS+=-framework OpenGL
else
	LDFLAGS+=-lGL
endif
LDFLAGS+= -lProcessCollector -lshout

libboids2d.so: $(OBJS)
	$(CC) -o $@ $(CFLAGS_SO_CREATE) $(LIB_PATH) $^ $(LDFLAGS)
	mv *.so ../bin

all: libboids2d.so
			
clean:
	rm -f *.o
	rm -f *.so

%.o:%.cpp
	$(CC) -g -Wno-multichar $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) -c -o $@ $<

        
depend: .depend
.depend:
	$(CC) -g -Wno-multichar $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) -MM $(SRC) > $@

-include .depend
        