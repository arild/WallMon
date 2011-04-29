SRC_DIR=~/WallMon/lib/stubs
DST_DIR=~/WallMon/dispatcher
protoc -I=$SRC_DIR --python_out=$DST_DIR $SRC_DIR/MonitorDispatcher.proto
