include ../../Makefile.inc

%.o:%.cc
	$(CC) -c -fPIC -o $@ $(INC_PATH) $<

MAKE_PROTOBUF_MESSAGE_LIB=ar cru lib$@Message.a $@.pb.o

all: GenerateStubs
	make _all
	
_all: MonitorDispatcher ProcessCollector Wallmon TableState
	mv *.a ..

clean:
	${CLEAN}
	
GenerateStubs:
	protoc --cpp_out=. *.proto

MonitorDispatcher: MonitorDispatcher.pb.o
	${MAKE_PROTOBUF_MESSAGE_LIB}
		
ProcessCollector: ProcessCollector.pb.o
	${MAKE_PROTOBUF_MESSAGE_LIB}

Wallmon:  Wallmon.pb.o
	${MAKE_PROTOBUF_MESSAGE_LIB}

TableState:  TableState.pb.o
	${MAKE_PROTOBUF_MESSAGE_LIB}

include ../../Makefile.dep.inc