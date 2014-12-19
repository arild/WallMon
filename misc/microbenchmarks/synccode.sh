#!/bin/bash
rsync -rvu --copy-links ../microbenchmarks ani027@ice.cs.uit.no:./WallMon/misc
rsync -rvu --copy-links ../../lib/Stat.h ani027@ice.cs.uit.no:./WallMon/lib
rsync -rvu --copy-links ../../lib/LinuxProcessMonitorLight.h ani027@ice.cs.uit.no:./WallMon/lib
rsync -rvu --copy-links ../../lib/LinuxProcessMonitorLight.cpp ani027@ice.cs.uit.no:./WallMon/lib