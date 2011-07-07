include ../Makefile.inc

SRC=*.cpp *.h
CFLAGS= -Wno-multichar -rdynamic

LIB_LOCAL=-lByteBuffer -lMonitorDispatcher
ifeq ($(PLATFORM), Darwin)	
	LIB_LOCAL+=-framework OpenGL `sdl-config --libs`
endif

all: wallmons

wallmons: $(SRC)
	$(CC) $(CFLAGS) -D $(PLATFORM) -o $@ $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_COMMON) $(LIB_LOCAL)

clean:
	rm -f *.o
	rm -f wallmons

