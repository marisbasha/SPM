exec > ./data/logs/output_time.txt 2>&1
for n in $(cat ./data/config)
do
    echo $n
   ./seq 41 41 $(echo $n | cut -d ":" -f 1) $(echo $n | cut -d ":" -f 2) $(echo $n | cut -d ":" -f 3) | awk '{SUM+=$5;I+=1}; END {print SUM/I}'
done