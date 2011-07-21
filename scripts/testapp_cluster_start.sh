#!/bin/bash

FORK_CMD="cluster-fork --bg"
# Escaping how-to: http://tldp.org/LDP/Bash-Beginners-Guide/html/sect_03_03.html
if ["`$HOSTNAME`" == "`ice.cs.uit.no`"];
then
FORK_CMD="rocks run host"
fi

CMD="cd ~/apps/testapp && ./TestApp $@"
${FORK_CMD} "${CMD}"

