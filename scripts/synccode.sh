#!/bin/bash
SRC="/home/arild/WallMon"
EXCLUDE_FILE=rsync_exclude

DEST=arild@rocksvv.cs.uit.no:.
rsync -rvu --copy-links --exclude-from=$EXCLUDE_FILE $SRC $DEST
rsync -rvu $SRC/../apps $DEST
rsync -rvu ~/src $DEST

DEST=ani027@ice.cs.uit.no:.
rsync -rvu --copy-links --exclude-from=$EXCLUDE_FILE $SRC $DEST
rsync -rvu $SRC/../apps $DEST
rsync -rvu ~/src $DEST

