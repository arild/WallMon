set terminal postscript enhanced eps color
set output 'collector_time_alignment_daemon.ps'

set xlabel 'Hz (Number of samples per second)'
set ylabel 'Alignment difference in milliseconds'
set key left top

# Notation: 'w l' = with lines, 'lc' = linecolor, 'lw' = linewidth, 'lt' = linetype
plot '../data/collector_time_alignment_daemon.dat' using 1:3 w lp lt 1 pt 2 lc 1 title 'alignment difference', \
					      '' using 1:3:5:xticlabels(2) w yerrorbars lt 1 pt 1 lc 1 title 'standard deviation', \
                                              '' using 1:7 w lp lt 1 pt 2 lc 3 title 'maximum values'
