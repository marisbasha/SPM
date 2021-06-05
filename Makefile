LINKERFLAG = -lm
seq:
	g++-11 -std=c++2a seq.cpp -o seq
seqv1:
	g++-11 -std=c++2a seq.v1.cpp -o seqv1
par:
	g++-11 -std=c++2a par.cpp -o par -pthread -O3 
parv1:
	g++-11 -std=c++2a par.v1.cpp -o parv1 -pthread -O3 
ff:
	make -f Makefile.ff
grp:
	g++-11 -std=c++2a graphgen.cpp -o graphgen