TARGET="/home/arild/Dropbox/thesis/WallMon" # Use full path for safety reasons
find $TARGET -type f -name *.o -exec rm -f {} \;
find $TARGET -type f -name *.so -exec rm -f {} \;
find $TARGET -type f -name *.a -exec rm -f {} \;
rm -f $TARGET/daemon/wallmond
rm -f $TARGET/server/wallmons
