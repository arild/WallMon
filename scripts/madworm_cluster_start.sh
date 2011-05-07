#!/bin/bash
WMON=/home/arild/WallMon
CMD="cd ${WMON}/../apps/madworm && python madworm.py $@"

cluster-fork --bg ${CMD}
