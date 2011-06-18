include ../../Makefile.inc

CFLAGS = -O2 -c -fPIC $(INC_PATH) $(LIB_PATH) -lprotobuf

all: GenerateStubs MonitorDispatcher ProcessCollector Wallmon
	mv *.a ..

clean:
	rm *.h
	rm *.cc
	rm *.o
	
GenerateStubs: *.proto
	protoc --cpp_out=. *.proto 

MonitorDispatcher:
	$(CC) $(CFLAGS) $@.pb.cc
	ar rcs lib$@Message.a $@.pb.o
	
ProcessCollector:
	$(CC) $(CFLAGS) $@.pb.cc
	ar rcs lib$@Message.a $@.pb.o

Wallmon:
	$(CC) $(CFLAGS) $@.pb.cc
	ar rcs lib$@Message.a $@.pb.o