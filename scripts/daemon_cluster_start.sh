#!/bin/bash
DAEMON_DIR=/home/arild/WallMon/daemon
DAEMON_EXECUTE="./wallmond -d"

cluster-fork --bg "cd ${DAEMON_DIR} && ${DAEMON_EXECUTE}"
cd ${DAEMON_DIR} && ${DAEMON_EXECUTE} && cd -


