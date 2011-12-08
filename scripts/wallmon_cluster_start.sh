#!/bin/bash
. $(dirname $0)/incl.sh
. $(dirname $0)/server_cluster_start.sh
. $(dirname $0)/daemon_cluster_start.sh

# The delay makes sure both server and client is up and running when the 
# initilization broadcoast message from the dispatcher arrives
sleep 0.1
${WALLMON_DISPATCH_MODULES}