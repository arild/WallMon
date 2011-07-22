CC=g++ -O2

LIB_COMMON= -lMonitorDispatcher -lConfig -lpthread -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lwSystem -lWallmonMessage -lIoLogger

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

# Flags for building dynamic libraries
CFLAGS_SO_PREPARE = -c -fPIC
CFLAGS_SO_CREATE = -shared
ifeq ($(PLATFORM), Darwin)	
	CFLAGS_SO_CREATE+=-undefined dynamic_lookup
endif

# Global paths for libraries required by Wallmon
WALLMON=$(HOME_PATH)/WallMon
SYSTEM=$(HOME_PATH)

INC_PATH=-I$(SYSTEM)/include -I$(SYSTEM)/include/freetype2 -I$(WALLMON)/lib
LIB_PATH=-L$(SYSTEM)/lib -L$(WALLMON)/lib
CLEAN=rm -f *.o ; rm -f *.gch ; rm -f *.a
