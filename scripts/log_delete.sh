#!/bin/bash
. $(dirname $0)/incl.sh
CMD="rm -f /tmp/*wallmon*"
${CLUSTER_FORK} "${CMD}"
${CMD}
