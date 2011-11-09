set terminal postscript enhanced eps color
set output 'wallmon_network_latency.ps'

set xlabel 'Hz (Number of samples per second)'
set ylabel 'Latency in milliseconds'

#set yrange [0:55]

# Notation: 'w l' = with lines, 'lc' = linecolor, 'lw' = linewidth, 'lt' = linetype
plot '../data/wallmon_packet_latency_msec.dat' using 1:3 w lp lt 1 pt 2 lc 1 title 'latency', \
					      '' using 1:3:5:xticlabels(2) w yerrorbars lt 1 pt 1 lc 1 title 'standard deviation', \
                                              '' using 1:6 w lp lt 1 pt 2 lc 2 title 'minimum values', \
                                              '' using 1:7 w lp lt 1 pt 2 lc 3 title 'maximum values'
