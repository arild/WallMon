CC = g++
CFLAGS_COMMON = -Wno-multichar -g

LIB_COMMON= -lMonitorDispatcher -lConfig -lpthread -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lSystem -lWallmonMessage -lIoLogger

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

INC_PATH=-I$(SYSTEM)/include -I$(SYSTEM)/include/freetype2 -I$(WALLMON)/lib
LIB_PATH=-L$(SYSTEM)/lib -L$(SYSTEM)/lib64 -L$(WALLMON)/lib
