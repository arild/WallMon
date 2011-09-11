. $(dirname $0)/incl.sh
START_CMD="cd ${APP_DIR}/testapp && ./testapp $@"
${CLUSTER_FORK} "$START_CMD"
$START_CMD

