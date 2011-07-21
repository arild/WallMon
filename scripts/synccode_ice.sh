#!/bin/bash
SRC="/home/arild/WallMon"
EXCLUDE_FILE=${SRC}/scripts/rsync_exclude

DEST=ani027@ice.cs.uit.no:.
rsync -rvu --copy-links --exclude-from=$EXCLUDE_FILE $SRC $DEST
rsync -rvu ~/src $DEST

