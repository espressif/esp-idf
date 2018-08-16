# this is a GNUPLOT script generating the figure of spi_master_freq_tv.png

set xlabel "Input delay (ns)"
set xrange [0: 125]
set ylabel "Fmax (MHz)"
set yrange [0: 81]
set xtics 12.5 textcolor rgb "black"
set ytics 10 textcolor rgb "black"
set border 3 lc rgb "gray" lw 2
set grid lt -1 lc rgb "gray" lw 2
set samples 10000
set terminal png size 700,500
set output "plot.png"

apb = 12.5

#each line is broken into 10 pieces by the range determined by i
f1(i,x) = (x>= i*apb) && (x < (i+1)*apb) ? 80./(i+1) : 1/0

set style circle radius graph 0.008

#solid and empty circles are draw by the coordinates given in the csv
plot [0:125]\
    f1(-1, x) lw 3lc rgb "blue" title "IOMUX",\
    for [i=0:9] f1(i, x) with lines lw 3 lc rgb "blue" notitle,\
    f1(0, x+25) lw 3 lc rgb "red" title "GPIO",\
    for [i=2:11] f1(i, x+25) with lines lw 3 lc rgb "red" notitle, \
    "tv.csv" using 1:2 with circles notitle fill solid fc rgb "blue", \
    "tv.csv" using 1:4 with circles notitle fc rgb "blue",\
    "tv.csv" using 1:3 with circles notitle fill solid fc rgb "red" ,\
    "tv.csv" using 1:5 with circles notitle fc rgb "red" 
    

