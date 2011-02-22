WMON=/home/arild/WallMon
cluster-fork --nodes="tile-0-%d:0-1" --bg "cd ${WMON}/daemon && ./wallmond -d"
