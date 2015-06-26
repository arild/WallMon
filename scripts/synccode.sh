#!/bin/sh
. $(dirname $0)/incl.sh

echo "Syncing" ${WMON_DIR} "to" ${DEST}
${RSYNC} ${WMON_DIR} arild@${ROCKSVV}:.
