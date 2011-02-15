# Delete all .o, .so and binary files. 
# They need to be compiled on rocksvv
sh cleanup.sh
DEST=arild@rocksvv.cs.uit.no:.
SRC="/home/arild/Dropbox/thesis/WallMon"
rsync -rvu $SRC $DEST
rsync -rvu ~/src $DEST
