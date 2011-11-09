set terminal postscript enhanced eps color
set output 'wallmon_sample_duration.ps'

set xlabel 'Hz (Number of samples per second)'
set ylabel 'Execution time in milliseconds'

#set yrange [0:55]

# Notation: 'w l' = with lines, 'lc' = linecolor, 'lw' = linewidth, 'lt' = linetype
plot '../data/wallmond_sample_duration_msec.dat' using 1:3 w lp lt 1 pt 2 lc 1 title 'sample duration', \
					      '' using 1:3:5:xticlabels(2) w yerrorbars lt 1 pt 1 lc 1 title 'standard deviation', \
                                              '' using 1:6 w lp lt 1 pt 2 lc 2 title 'minimum values', \
                                              '' using 1:7 w lp lt 1 pt 2 lc 3 title 'maximum values'
