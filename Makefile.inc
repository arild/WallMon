CC=g++ -O2 -g -Wno-multichar

# Determine the path to the home directory
USER = $(shell whoami)
PLATFORM := $(shell uname)
ifeq ($(PLATFORM), Linux)
	HOME_DIR=home
endif
ifeq ($(PLATFORM), Darwin)
	HOME_DIR=Users
endif
HOME_PATH=/$(HOME_DIR)/$(USER)

# Global paths for libraries required by Wallmon
WALLMON=$(HOME_PATH)/WallMon
SYSTEM=$(HOME_PATH)
INC_PATH=-I$(SYSTEM)/include -I$(SYSTEM)/include/freetype2 -I$(WALLMON)/lib -I/share/apps/include/ -I/share/apps/cuda/sdk/shared/inc/ 
LIB_PATH=-L$(SYSTEM)/lib -L$(WALLMON)/lib -L/share/apps/lib/ -ljpeg -lcurl -lpthread -lGLEW -lGL -lGLU -lX11 -lGLEW

# Default target for compiling single source files to object files (no linking)
# If this target is not suitable, simply overwrite it
%.o:%.cpp
	$(CC) -c -o $@ $(INC_PATH) $<

# Convenient variables
SRC = $(shell find . -name \*.cpp)
OBJS = $(SRC:.cpp=.o)
CLEAN=rm -f *.o ; rm -f *.gch ; rm -f *.a ; rm -f *.so

# Flags for building dynamic libraries
CFLAGS_SO_PREPARE = -c -fPIC
CFLAGS_SO_CREATE = -shared
ifeq ($(PLATFORM), Darwin)	
	CFLAGS_SO_CREATE+=-undefined dynamic_lookup
endif


# Commands for building stating and shared libraries
MAKE_STATIC_LIB=ar cru lib$@.a $@.o
MAKE_SHARED_LIB=$(CC) -o lib$@.so $(CFLAGS_SO_CREATE) $(LIB_PATH) $@.o
