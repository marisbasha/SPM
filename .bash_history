rm par
make par
./par 41 41 4 10000 100 200
rm par
make par
./par 41 41 4 10000 100 200
rm par
make par
./par 41 41 4 10000 100 200
rm par
./par 41 41 4 10000 100 200
rm par
make par
./par 41 41 4 10000 100 200
rm par
make par
./par 41 41 4 10000 100 200
./par 41 41 4 10000 100 200rm par
rm par
make par
clear
./par 41 41 4 10000 100 200rm par
./par 41 41 4 10000 100 200
rm par
make par
rm par
rm seq
rm grp
rm ff
clear
make par
make par
make par
clear
make par
clear
make par
clear
make par
clear
clear
make par
clear
make par
clear
make par
clear
make par
clear
make par
clear
make par
clear
make par
clear
make par
clear
make par
./par make par
make par
./par 41 41 4 10000 100 200
clear
make par
rm par
make par
./par 41 41 4 10000 100 200
rm ff
make ff
make ff
clear
make ff
clear
make ff
clear
make ff
./ff 44 41 4 10000 100 200
./par 44 41 4 10000 100 200
rm seq
make seq
./seq 44 41  10000 100 200
./seq 44 41 10000 100 200
clear
rm seq && make seq
./seq 44 41 10000 100 200
rm seq && make seq
./seq 44 41 10000 100 200
rm seq
make seq -DWAIT
make seq ARGS="-DWAIT"
make seq -DWAIT
./seq 44 41 10000 100 200
clear
make blk
clear
make blk
make blk
./blk
make blk
clear
./blk
clear
make blk
./blk
clear
make par
rm par
make par
./ff 44 41 4 10000 100 200
./par 44 41 4 10000 100 200
./se 44 41 10000 100 200
./seq 44 41 10000 100 200
rm se
rm seq
make seq
./seq 44 41 10000 100 200
rm seq
./seq 44 41 10000 100 200 ARGS="-DWAIT&STUFF"
make seq ARGS="-DWAIT&STUFF"
make seq ARGS="-DWAIT -DSTUFF"
rm seq
make seq ARGS="-DWAIT -DSTUFF"
clear
make seq ARGS="-DWAIT -DWITHTIME"
rm seq
make seq ARGS="-DWAIT -DWITHTIME"
./seq 44 41 10000 100 200 ARGS="-DWAIT&STUFF
./seq 44 41 10000 100 200
rm seq
make seq ARGS="-DWAIT"
./seq 44 41 10000 100 200
rm par
make par
./par 44 41 4 10000 100 200 ARGS="-DWITHTIME"
rm par
./par 44 41 4 10000 100 200 ARGS="-DWITHTIME"
make par
./par 44 41 4 10000 100 200 ARGS="-DWITHTIME"
clear
rm par
make par
./par 44 41 4 10000 100 200 ARGS="-DWITHTIME"
rm par && make par ARGS="-DWITHTIME"
clear
rm par && make par ARGS="-DWITHTIME"
make par ARGS="-DWITHTIME"
./par 44 41 4 10000 100 200
grep Time to update x with locks
./par 44 41 4 10000 100 200 | grep Time to update x with locks
grep -n "time to update"
./par 44 41 4 10000 100 200 | grep "Time to update x with locks"
clear
./par 44 41 4 10000 100 200
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock"
clear
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" 
clear 
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" awk '{split($0,a," "); sum += a[6]} END {print sum}' 
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk '{split($0,a," "); sum += a[6]} END {print sum}' 
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk sum += $6} END {print sum}' 
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk sum += $6} END {print sum}
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk {sum += $6} END {print sum}
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk '{sum += $6} END {print sum}'
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk '{sum += $6} END {clearprint sum}'
clear
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk 'BEGIN {SUM=0}; {SUM=SUM+$6}; END {printf "%.3f\n", SUM}' file
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk 'BEGIN {SUM=0}; {SUM=SUM+$6}; END {printf "%.3f\n", SUM}'
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk 'BEGIN {SUM=0}; {SUM=SUM+$5}; END {printf "%.3f\n", SUM}'
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk 'BEGIN {SUM=0}; {SUM=SUM+$4}; END {printf "%.3f\n", SUM}'
clear
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" 
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk print $6 
clear
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk '{SUM+=$3}END{print SUM}'
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk '{SUM+=$6}END{print SUM}'
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk -F ' ' '{SUM+=$6}END{print SUM}'
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk -F ' ' '$6'
clear
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | awk '$6'
clear
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" exec > log.txt 2>&1
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" | exec > log.txt 2>&1
exec > log.txt 2>&1 | ./par 44 41 4 10000 100 200 | grep "Time to process edges with lock"
exec > log.txt 2>&1 | ./par 44 41 4 10000 100 200 | grep "Time to process edges with lock"
clear
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock" 
exec > log.txt 2>&1
./par 44 41 4 10000 100 200 | grep "Time to process edges with lock"
clear
clear
make ff
clear
cd data/sh/
sh script.sh
cd ..
cd ..
sh ./data/sh/script.sh
./data/
sh ./data/sh/script.sh
make seq ARGS="-DWITHTIME"
rm seq
make seq ARGS="-DWITHTIME"
rm seq
make seq ARGS="-DWITHTIME"
./seq 41 41 10000 100 200
clear
rm seq
make seq ARGS="-DWITHTIME"
./seq 41 41 10000 100 200
clear
rm seq
make seq ARGS="-DWITHTIME"
./seq 41 41 10000 100 200
./seq 41 41 10000 100 200
clear
rm seq
make seq ARGS="-DWITHTIME"
./seq 41 41 10000 100 200 | awk '{SUM += $4} END {print SUM}'
clear
./seq 41 41 10000 100 200 | awk -F'|' ' { for (i = 1; i <= NF; ++i) print i, $i; exit } '
./seq 41 41 10000 100 200 | awk -F' ' ' { for (i = 1; i <= NF; ++i) print i, $i; exit } '
clear
rm seq
make seq ARGS="-DWITHTIME"
./seq 41 41 10000 100 200 | awk -F' ' ' { for (i = 1; i <= NF; ++i) print i, $i; exit } '
./seq 41 41 10000 100 200 | awk -F' ' ' { print $5;} '
./seq 41 41 10000 100 200 | awk -F' ' ' { SUM+=$5} END {print SUM;} '
./seq 41 41 10000 100 200 | awk -F' ' ' { SUM+=$5; I+=1} END {print SUM/I;} '
./seq 41 41 50000 600 800 | awk -F' ' ' { SUM+=$5; I+=1} END {print SUM/I;} '
./seq 41 41 50000 800 1000 | awk -F' ' ' { SUM+=$5; I+=1} END {print SUM/I;} '
./seq 41 41 30000 200 400 | awk -F' ' ' { SUM+=$5; I+=1} END {print SUM/I;} '
./seq 41 41 10000 200 500 | awk -F' ' ' { SUM+=$5; I+=1} END {print SUM/I;} '
./seq 41 41 10000 400 500 | awk -F' ' ' { SUM+=$5; I+=1} END {print SUM/I;} '
./seq 41 41 30000 600 700 | awk -F' ' ' { SUM+=$5; I+=1} END {print SUM/I;} '
ls
cd ..
ls
cd spm
rm ff
make f
make ff
rm grp
make grp
./grp 1000 200 300
ls
cd data
la
ls
cd graphs/
ls
clear
