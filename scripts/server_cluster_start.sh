#!/bin/bash
SERVER_DIR=~/WallMon/server
SERVER_EXECUTE="./wallmons -d"
DISPLAY="DISPLAY=localhost:0"

cluster-fork --bg "cd ${SERVER_DIR} && ${DISPLAY} ${SERVER_EXECUTE}"

