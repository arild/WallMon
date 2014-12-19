set terminal postscript enhanced color
set key invert left top
set output "wallmon_sampling_rate.ps"

set style data histograms # Histograms, not dots
set style histogram rowstacked # Stacks the histograms on top of each other
set style fill solid noborder # Layout for histograms
set boxwidth 0.75 absolute # Turns histogram into bar chart 

set ylabel "CPU Utilization"
set xlabel "Hz (Number of samples per second)"
set xrange [-1:5]

#plot "../data/wallmond_cpu_system_util.dat" using 3:xtic(2) title "kernel-level",\
#     "../data/wallmond_cpu_user_util.dat" using 3:xtic(2) title "user-level"


#plot "../data/wallmond_cpu_system_util.dat" using 1:3:4:xtic(2) w boxerror title "kernel-level",\
#     "../data/wallmond_cpu_user_util.dat" using 1:3:4:xtic(2) w boxerror title "user-level"
