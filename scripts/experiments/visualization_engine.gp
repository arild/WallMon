set terminal postscript enhanced eps color
set output 'visualization_engine.ps'

set xlabel 'Sample number'
set ylabel 'Execution time in milliseconds'

set xrange [0:1000]
set yrange [0:10]

set xtics 200
set ytics 2

# Notation: 'w l' = with lines, 'lc' = linecolor, 'lw' = linewidth, 'lt' = linetype
plot 'visualization_engine.dat' using 1:2 w l lc 1 lw 2 lt 1 title "Clear Screen", \
	    		     '' using 1:3 w l lc 2 lw 2 lt 1 title "Run Scenes", \
	    	             '' using 1:4 w l lc 3 lw 2 lt 1 title "Process Events", \
	    		     '' using 1:5 w l lc 4 lw 2 lt 1 title "Run Event Callbacks", \
	    		     '' using 1:6 w l lc 5 lw 2 lt 1 title "Swap Frame Buffer", \
	    		     '' using 1:7 w l lc 6 lw 2 lt 1 title "Other"	
