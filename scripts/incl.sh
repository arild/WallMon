#!/bin/bash

# Common variables for WallMon related scripts

USER=/home/arild
WMON_DIR=${USER}/WallMon
DAEMON_DIR=${WMON_DIR}/daemon
SERVER_DIR=${WMON_DIR}/server
DISPATCHER_DIR=${WMON_DIR}/dispatcher
SCRIPTS_DIR=${WMON_DIR}/scripts
MODULES_DIR=${WMON_DIR}/modules
SRC_DIR=${USER}/src
APP_DIR=${USER}/apps

DAEMON_PROC_NAME=wallmond
SERVER_PROC_NAME=wallmons

LIB_PATH="LD_LIBRARY_PATH=${USER}/lib:${USER}/WallMon/lib:$LD_LIBRARY_PATH"

DAEMON_EXECUTE="./${DAEMON_PROC_NAME} -d"
SERVER_EXECUTE="./${SERVER_PROC_NAME} -d"
WALLMON_DISPATCH_MODULES="python ${DISPATCHER_DIR}/main.py init"

ROCKSVV=rocksvv.cs.uit.no
ROCKSVV_SSH=arild@${ROCKSVV}
 
ICE=ice.cs.uit.no
ICE_SSH=ani027@${ICE}


if [ $HOSTNAME = $ICE ]; then
	CLUSTER_FORK="rocks run host"	
elif [ $HOSTNAME = $ROCKSVV ]; then
	CLUSTER_FORK="cf"
	DAEMON_EXECUTE="DISPLAY=:0.0 ${LIB_PATH} ${DAEMON_EXECUTE}"
	SERVER_EXECUTE="DISPLAY=:0.0 ${LIB_PATH} ${SERVER_EXECUTE}"
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
RSYNC="rsync -rvu --copy-links --exclude-from=${RSYNC_EXCLUDE_FILE}"


