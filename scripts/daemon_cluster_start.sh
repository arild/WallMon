. $(dirname $0)/incl.sh
${CLUSTER_FORK} "cd ${DAEMON_DIR} && ${DAEMON_EXECUTE}"
cd ${DAEMON_DIR} && ${DAEMON_EXECUTE} && cd -