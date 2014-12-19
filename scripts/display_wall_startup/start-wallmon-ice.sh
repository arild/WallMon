#!/bin/bash
SCRIPTS="/home/arild/WallMon/scripts"
${SCRIPTS}/server_cluster_start.sh
ssh arild@ice.cs.uit.no "${SCRIPTS}/daemon_cluster_start.sh"
