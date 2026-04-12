reset
set datafile separator ","

set terminal pngcairo size 1200,600 enhanced font "Verdana,14"
set output "temp.png"

set title "Temperature(Time)"
set xlabel "Time, s"
set ylabel "Temperature, °C"
set grid
set key top left

plot "cpu-temp.csv" using 1:2 with lines lw 2 lc rgb "#4C78A8" title "Temp"