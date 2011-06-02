#!/bin/bash
DAEMON_DIR=~/WallMon/daemon
DAEMON_EXECUTE="./wallmond -d"
FORK_CMD="cluster-fork --bg"

# Escaping how-to: http://tldp.org/LDP/Bash-Beginners-Guide/html/sect_03_03.html
if ["`$HOSTNAME`" == "`ice.cs.uit.no`"];
then
FORK_CMD="rocks run host"
fi

cd ${DAEMON_DIR} && ${DAEMON_EXECUTE} && cd -
${FORK_CMD} "cd ${DAEMON_DIR} && ${DAEMON_EXECUTE}"

