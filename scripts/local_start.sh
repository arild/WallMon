WMON=~/WallMon
cd $WMON/daemon ; ./wallmond -d ; cd -
cd $WMON/server ; ./wallmons -d ; cd -
cd $WMON/dispatcher ; python main.py init ; cd -
