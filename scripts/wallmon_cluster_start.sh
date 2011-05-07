#!/bin/bash
DAEMON_DIR=/home/arild/WallMon/daemon
DAEMON_EXECUTE="./wallmond -d"

SERVER_DIR=/home/arild/WallMon/server
SERVER_EXECUTE="./wallmons -d"

DISPLAY="DISPLAY=localhost:0"

cluster-fork --bg "cd ${SERVER_DIR} && ${DISPLAY} ${SERVER_EXECUTE} && cd ${DAEMON_DIR} && ${DAEMON_EXECUTE}"
cd ${DAEMON_DIR} && ${DAEMON_EXECUTE} cd -
