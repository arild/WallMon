WMON=/home/arild/WallMon
cluster-fork --nodes="tile-0-%d:0-0" --bg "cd ${WMON}/mad_worm && ./mad_worm"
