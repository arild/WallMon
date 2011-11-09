. $(dirname $0)/incl.sh
. $(dirname $0)/get_gnuplot_data_from_rocksvv.sh
cd ${MODULES_DIR}/gnuplot/scripts
find *.gp -exec gnuplot {} \;
find *.ps -exec ps2pdf {} \;
find *.pdf -exec pdfcrop {} {} \;
rm *.ps
find *.pdf -exec cp {} ~/thesis/report/figures/gnuplot_module \;

