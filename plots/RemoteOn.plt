set xtics 2
set grid
set yrange [-0.2:5.2]
plot "RemoteOn.test" w step, "RemoteOn.csv" using ($2*-10) w l

pause mouse close 
