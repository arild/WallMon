. $(dirname $0)/incl.sh

# Synchronizes WallMon source code with provided destinations

for arg in "$@"
do
	if [ $arg == "ice" ]; then
		${RSYNC} ${WMON_DIR} ${ICE}
		${RSYNC} ${SRC_DIR} ${ICE}
	elif [ $arg == "rocksvv" ]; then
		${RSYNC} ${WMON_DIR} ${ROCKSVV}
		${RSYNC} ${SRC_DIR} ${ROCKSVV}
	else
		echo "argument not defined: ${arg}"
	fi
done