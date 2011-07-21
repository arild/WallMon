NAME="TestApp"

FORK_CMD="cluster-fork --bg"
# Escaping how-to: http://tldp.org/LDP/Bash-Beginners-Guide/html/sect_03_03.html
if ["`$HOSTNAME`" == "`ice.cs.uit.no`"];
then
FORK_CMD="rocks run host"
fi

${FORK_CMD} "pkill -9 -f $NAME"
