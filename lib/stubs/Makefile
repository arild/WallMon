include ../../Makefile.inc

%.o:%.cc
	$(CC) -c -fPIC -o $@ $(INC_PATH) $<

MAKE_PROTOBUF_MESSAGE_LIB=ar cru lib$@Message.a $@.pb.o

all: GenerateStubs MonitorDispatcher ProcessCollector Wallmon
	mv *.a ..

clean:
	${CLEAN}
	
GenerateStubs:
	protoc --cpp_out=. *.proto

MonitorDispatcher: GenerateStubs MonitorDispatcher.pb.o
	${MAKE_PROTOBUF_MESSAGE_LIB}
		
ProcessCollector: GenerateStubs ProcessCollector.pb.o
	${MAKE_PROTOBUF_MESSAGE_LIB}

Wallmon: GenerateStubs Wallmon.pb.o
	${MAKE_PROTOBUF_MESSAGE_LIB}

include ../../Makefile.dep.inc