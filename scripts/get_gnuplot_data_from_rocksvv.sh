. $(dirname $0)/incl.sh
DATA_PATH="${MODULES_DIR}/gnuplot/data"
scp ${ROCKSVV_SSH}:${DATA_PATH}/* ${DATA_PATH}