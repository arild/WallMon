CC = g++
CFLAGS_COMMON = -Wno-multichar -rdynamic -fno-stack-protector -g

LIB_COMMON= -lMonitorDispatcher -lConfig -lpthread -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lSystem -lWallmonMessage -lIoLogger

USER = $(shell whoami)
WALLMON=/home/$(USER)/WallMon
SYSTEM=/home/$(USER)

INC_PATH=-I$(SYSTEM)/include -I$(WALLMON)/lib
LIB_PATH=-L$(SYSTEM)/lib -L$(SYSTEM)/lib64 -L$(WALLMON)/lib