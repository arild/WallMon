set terminal postscript enhanced color

set style data histograms # Histograms, not dots
set style fill solid noborder # Layout for histograms
set boxwidth 0.75 absolute # Turns histogram into bar chart

set ylabel 'Memory usage in megabytes'
set xlabel 'Hz (Number of samples per second)'
set key invert left top

set output 'wallmon_sampling_rate_memory.ps'
plot '../data/wallmond_memory_megabytes.dat' using 3:xtic(2) title 'WallMon Client',\
     '../data/wallmons_memory_megabytes.dat' using 3:xtic(2) title 'WallMon Server'

