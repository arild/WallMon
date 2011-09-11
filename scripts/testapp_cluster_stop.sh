. $(dirname $0)/incl.sh
KILL_CMD="pkill -9 -f testapp $@"
${CLUSTER_FORK} "$KILL_CMD"
$KILL_CMD