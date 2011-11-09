set terminal postscript enhanced eps color
set output 'wallmon_server_network.ps'

set xlabel 'Time in seconds'
set ylabel 'Bandwidth in Mbytes/s'

# Notation: 'w l' = with lines, 'lc' = linecolor, 'lw' = linewidth, 'lt' = linetype
plot '../data/wallmons_network_receive_megabytes.dat' using 1:2 w lines lc 1 title 'WallMon server bandwidth'
