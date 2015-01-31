#!/bin/sh
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

	echo ${RSYNC}
	echo ${WMON_DIR}
	echo ${DEST}
	${RSYNC} ${WMON_DIR} ${DEST}:.
	if [ $REMOTE_BUILD == true ]; then
		ssh ${DEST} "cd WallMon && make && exit"
	fi
done