#!/bin/bash

# Common variables for WallMon related scripts

WMON_DIR=~/WallMon
DAEMON_DIR=${WMON_DIR}/daemon
SERVER_DIR=${WMON_DIR}/server
DISPATCHER_DIR=${WMON_DIR}/dispatcher
SRC_DIR=~/src
APP_DIR=~/apps

DAEMON_PROC_NAME=wallmond
SERVER_PROC_NAME=wallmons

DAEMON_EXECUTE="./${DAEMON_PROC_NAME} -d"
SERVER_EXECUTE="./${SERVER_PROC_NAME} -d"
WALLMON_DISPATCH_MODULES="python ${DISPATCHER_DIR}/main.py init"

ROCKSVV_ADDR=rocksvv.cs.uit.no
ROCKSVV=arild@${ROCKSVV_ADDR}:.

ICE_ADDR=ice.cs.uit.no
ICE=ani027@${ICE_ADDR}:.

if [ $HOSTNAME = $ICE_ADDR ]; then
	CLUSTER_FORK="rocks run host"	
elif [ $HOSTNAME = $ROCKSVV_ADDR ]; then
	CLUSTER_FORK=cf
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


