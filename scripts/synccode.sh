. $(dirname $0)/incl.sh

# Synchronizes WallMon source code and related artifacts with provided destinations
REMOTE_BUILD=false
DEST=""
for arg in "$@"
do
	if [ $arg == "c" ]; then
		REMOTE_BUILD=true
		continue
	elif [ $arg == "ice" ]; then
		DEST=arild@${ICE}
	elif [ $arg == "rocksvv" ]; then
		DEST=arild@${ROCKSVV}
	else
		echo "argument not defined: ${arg}"
		continue
	fi

	${RSYNC} ${WMON_DIR} ${DEST}:.
	${RSYNC} ${SRC_DIR} ${DEST}:.
	${RSYNC} ${APP_DIR} ${DEST}:.
	if [ $REMOTE_BUILD == true ]; then
		ssh ${DEST} "cd WallMon && make && exit"
	fi
done