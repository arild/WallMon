set terminal postscript enhanced color

set style data histograms # Histograms, not dots
set style histogram rowstacked # Stacks the histograms on top of each other
set style fill solid noborder # Layout for histograms
set boxwidth 0.75 absolute # Turns histogram into bar chart 

set ylabel 'CPU Utilization'
set xlabel 'Hz (Number of samples per second)'
set key invert left top

set output 'wallmond_sampling_rate_cpu.ps'
plot '../data/wallmond_cpu_system_util.dat' using 3:xtic(2) title 'kernel-level',\
     '../data/wallmond_cpu_user_util.dat' using 3:xtic(2) title 'user-level'

set output 'wallmons_sampling_rate_cpu.ps'
plot '../data/wallmons_cpu_system_util.dat' using 3:xtic(2) title 'kernel-level',\
     '../data/wallmons_cpu_user_util.dat' using 3:xtic(2) title 'user-level'
