set terminal postscript enhanced color
set key invert left top
set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics;
set output "wallmon_sampling_rate.ps
set boxwidth 0.75 absolute
set style fill solid 1.00 border -1
set style histogram rowstacked
set style data histograms
set ylabel "CPU Utilization";
set xlabel "Hz (Number of samples per second)"
plot "wallmon_sampling_rate.ps" using 2 t "kernel-level", '' using 3:xtic(1) t "user-level";
