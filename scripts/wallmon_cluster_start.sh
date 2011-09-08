#!/bin/bash
. $(dirname $0)/incl.sh
. $(dirname $0)/server_cluster_start.sh
. $(dirname $0)/daemon_cluster_start.sh

# Time to make sure both server and client is up and running when the 
# initilization broadcoast message from the dispatcher comes
sleep 0.5 
${WALLMON_DISPATCH_MODULES}