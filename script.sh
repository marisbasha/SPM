exec > log.txt 2>&1
for n in $(cat config)
do
   ./seq 41 41 $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3)
   for w in 1 2 4 8 16 32 64 128 256
   do
      for s in {1..10}
      do
         ./par 41 41 ${w} $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3)
         ./ff 41 41 ${w} $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3)
         ./ff 41 41 ${w} $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3) farm
      done
   done
done