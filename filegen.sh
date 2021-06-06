for n in $(cat config)
do
   ./grp $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3)
done