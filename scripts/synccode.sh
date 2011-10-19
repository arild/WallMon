. $(dirname $0)/incl.sh

# Synchronizes WallMon source code and related artifacts with provided destinations
REMOTE_BUILD=false
ADDR=""
ADDR_SSH=""
for arg in "$@"
do
	if [ $arg == "c" ]; then
		REMOTE_BUILD=true
		continue
	elif [ $arg == "ice" ]; then
		ADDR=${ICE}
		ADDR_SSH=${ICE_SSH}
	elif [ $arg == "rocksvv" ]; then
		ADDR=${ROCKSVV}
		ADDR_SSH=${ROCKSVV_SSH}
	else
		echo "argument not defined: ${arg}"
		continue
	fi

	${RSYNC} ${WMON_DIR} ${ADDR_SSH}:.
	${RSYNC} ${SRC_DIR} ${ADDR_SSH}:.
	${RSYNC} ${APP_DIR} ${ADDR_SSH}:.
	if [ $REMOTE_BUILD == true ]; then
		ssh ${ADDR_SSH} "cd WallMon && make && exit"
	fi
done