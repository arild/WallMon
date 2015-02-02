include ../../Makefile.inc

# Take display wall into account on Rocksvv cluster
HOSTNAME = $(shell hostname)
ifeq ("$(HOSTNAME)", "rocksvv")
ROCKSVV = -D ROCKSVV
endif

LIBS=-lm -lConfig -lSystem -lSTouchManager -lWallView -lpthread -lboost_thread -lboost_system -lglog -lfreetype -lz -lftgl `sdl-config --libs` -lSDL_image -lGL -lProcessCollector -lshout -lTableStateMessage -lIL

all: libboids2d.so
			
clean:
	${CLEAN}
	
%.o:%.cpp
	$(CC) -o $@ $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) $<

libboids2d.so: $(OBJS)
	$(CC) -o $@ $(CFLAGS_SO_CREATE) $(LIB_PATH) $^ $(LIBS)
	mv *.so ../bin

include ../../Makefile.dep.inc
        