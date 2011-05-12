#!/bin/bash
WMON=/home/arild/WallMon
CMD="cd ${WMON}/../apps/madworm && ./madworm $@"

cluster-fork --bg ${CMD}
