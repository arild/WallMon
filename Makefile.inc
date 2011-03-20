CC = g++
CFLAGS_COMMON = -rdynamic -fno-stack-protector -Wall

LIB_COMMON=-lpthread -lboost_thread -ldl -lev -lglog -lprotobuf -lPracticalSocket -lClassLoader -lSystem -lWallmonMessage -lIoLogger #-lprofiler

WALLMON=/home/arild/WallMon
SYSTEM=/home/arild

INC_PATH=-I$(SYSTEM)/include -I$(WALLMON)/lib
LIB_PATH=-L$(SYSTEM)/lib -L$(WALLMON)/lib