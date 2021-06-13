# Code

Build the code

```bash
make grp #Make graph generator

make seq #Make sequential code
make seq ARGS="-DWITHTIME" #Make sequential code with time measures printed
make seq ARGS="-DWAIT" #Make sequential code with active wait
make seq ARGS="-DWITHTIME -DWAIT" #Make sequential code with both

make par #Make sequential code
make par ARGS="-DWITHTIME" #Make parallel code with time measures printed
make par ARGS="-DWAIT" #Make parallel code with active wait
make par ARGS="-DWITHTIME -DWAIT" #Make parallel code with both

make ff #Make sequential code
make ff ARGS="-DWITHTIME" #Make fastflow code with time measures printed
make ff ARGS="-DWAIT" #Make fastflow code with active wait
make ff ARGS="-DWITHTIME -DWAIT" #Make fastflow code with both

```

Run the code

```bash
./grp {number of nodes in graph} {min edges} {max edges to be added}
./grp 10000 100 200 #Example

./seq {start node} {value to be counted} {number of nodes in graph} {min edges} {max edges to be added}
./seq 41 41 10000 100 200 #Example

./par {start node} {value to be counted} {number of workers} {number of nodes in graph} {min edges} {max edges to be added}
./par 41 41 32 10000 100 200 #Example

./ff {start node} {value to be counted}  {number of workers} {number of nodes in graph} {min edges} {max edges to be added} {farm if farm}
./ff 41 41 32 10000 100 200 #Example just workers
./ff 41 41 32 10000 100 200 farm #Example farm
```

Utility sh scripts

```bash
sh data/sh/script.sh #Run all possible examples for configurations
```