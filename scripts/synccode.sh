#!/bin/bash
DEST=arild@rocksvv.cs.uit.no:.
SRC="/home/arild/WallMon"
EXCLUDE_FILE=rsync_exclude

rsync -rvu --copy-links --exclude-from=$EXCLUDE_FILE $SRC $DEST
rsync -rvu $SRC/../apps $DEST
rsync -rvu ~/src $DEST
