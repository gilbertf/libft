set xtics 4
set grid
set yrange [-0.2:5.2]
plot "RemoteOff.test" w step, "RemoteOff.csv" using ($2*-10) w l

pause mouse close 

