. $(dirname $0)/../incl.sh
# Local storage
mkdir ~/charts
rm -rf ~/charts/*

# Retrieve postscript files
#CMD="find -H WallMon -regex \".*\.\(ps\|dat\)\" -type f -exec cp -f {} ~/charts \;"
CMD="find -H WallMon -name \"*.dat\" -type f -exec mv -f {} ~/charts \;"
ssh ${ROCKSVV_SSH} "mkdir ~/charts ; rm -rf ~/charts/* ; ${CMD}"
scp ${ROCKSVV_SSH}:~/charts/* ~/charts

# Convert, crop and remove old
cd ~/charts
gnuplot ${SCRIPTS_DIR}/visualization_engine.gp visualization_engine.dat
find *.ps -exec ps2pdf {} \;
find *.pdf -exec pdfcrop {} {} \;
rm *.ps
