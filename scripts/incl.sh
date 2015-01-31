#!/bin/bash

# Common variables for WallMon related scripts

HOME_DIR=/home/arild
SRC_DIR=${HOME_DIR}/src
APP_DIR=${HOME_DIR}/apps
WMON_DIR=${HOME_DIR}/WallMon

DAEMON_DIR=${WMON_DIR}/daemon
SERVER_DIR=${WMON_DIR}/server
DISPATCHER_DIR=${WMON_DIR}/dispatcher
SCRIPTS_DIR=${WMON_DIR}/scripts
MODULES_DIR=${WMON_DIR}/modules

DAEMON_PROC_NAME=wallmond
SERVER_PROC_NAME=wallmons

export LD_LIBRARY_PATH="${HOME_DIR}/lib:${WMON_DIR}/lib:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/:/home/arild/include/:
export DISPLAY=:0.0

DAEMON_EXECUTE="${DAEMON_DIR}/${DAEMON_PROC_NAME} -d"
SERVER_EXECUTE="${SERVER_DIR}/${SERVER_PROC_NAME} -d"
WALLMON_DISPATCH_MODULES="python ${DISPATCHER_DIR}/main.py init"

ROCKSVV=rocksvv.cs.uit.no
ICE=ice.cs.uit.no

if [ $HOSTNAME = $ICE ]; then
	CLUSTER_FORK="rocks run host"	
elif [ $HOSTNAME = $ROCKSVV ]; then
	CLUSTER_FORK="/share/apps/bin/cf"
fi

if [ $# -eq 0 ]; then
	DAEMON_KILL="pkill -SIGTERM $DAEMON_PROC_NAME"
	SERVER_KILL="pkill -SIGTERM $SERVER_PROC_NAME"
else
	# Assume user provides arguments to pkill, such as -9 (SIGKILL) 
	DAEMON_KILL="pkill $@ $DAEMON_PROC_NAME"
	SERVER_KILL="pkill $@ $SERVER_PROC_NAME"
fi

RSYNC_EXCLUDE_FILE=${WMON_DIR}/scripts/rsync_exclude
RSYNC="rsync -azv --copy-links --exclude-from=${RSYNC_EXCLUDE_FILE}"

