set xtics 8
set grid
set yrange [-0.2:5.2]
plot "PowerOn.test" w step, "PowerOn.csv" using ($2*-10) w l

pause mouse close 
