include ../Makefile.inc



SRC=*.cpp *.h

LIB_LOCAL=-lByteBuffer -lMonitorDispatcher
ifeq ($(PLATFORM), Darwin)	
	LIB_LOCAL+=-framework OpenGL `sdl-config --libs`
endif

all: wallmons

wallmons: $(SRC)
	$(CC) $(CFLAGS_COMMON) -D $(PLATFORM) -o $@ $(SRC) $(INC_PATH) $(LIB_PATH) $(LIB_COMMON) $(LIB_LOCAL)

clean:
	rm -f *.o
	rm -f wallmons

