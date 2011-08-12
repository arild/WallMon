. $(dirname $0)/incl.sh
cd $DAEMON_DIR ; $DAEMON_EXECUTE ; cd -
cd $SERVER_DIR ; $SERVER_EXECUTE ; cd -
cd $DISPATCHER_DIR ; python main.py init ; cd -