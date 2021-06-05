
# exec > log.txt 2>&1
echo "Running seq"
for n in $(cat config)
do
   ./seq 41 41 $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3)
   for w in 1 2 4 6 7
   do
      echo ${w}
      #./par 41 41 ${w} $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3)
      ./ff 41 41 ${w} $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3)
   done
done