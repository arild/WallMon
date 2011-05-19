CC = g++
CFLAGS_COMMON = -Wno-multichar -rdynamic -fno-stack-protector

LIB_COMMON=-lMonitorDispatcher -lpthread -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lSystem -lWallmonMessage -lIoLogger

WALLMON=/home/arild/WallMon
SYSTEM=/home/arild

INC_PATH=-I$(SYSTEM)/include -I$(WALLMON)/lib
LIB_PATH=-L$(SYSTEM)/lib -L$(WALLMON)/lib