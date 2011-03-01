#!/bin/bash
WMON=/home/arild/WallMon
CMD="cd ${WMON}/../apps/mad_worm && ./mad_worm"

cluster-fork --bg ${CMD}
