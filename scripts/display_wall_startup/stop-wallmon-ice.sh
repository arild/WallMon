#!/bin/bash
SCRIPTS="/home/arild/WallMon/scripts"
ssh arild@ice.cs.uit.no "${SCRIPTS}/daemon_cluster_stop.sh"
${SCRIPTS}/server_cluster_stop.sh

