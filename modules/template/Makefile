include ../../Makefile.inc

%.o:%.cpp
	$(CC) -o $@ $(ROCKSVV) $(CFLAGS_SO_PREPARE) $(INC_PATH) $<

all: test.so

test.so: $(OBJS)
	$(CC) -o $@ $(CFLAGS_SO_CREATE) $(LIB_PATH) $^
	mv $@ ../bin
			
clean:
	${CLEAN}

include ../../Makefile.dep.inc