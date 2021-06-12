ifndef FF_ROOT
FF_ROOT		= $(HOME)/fastflow
endif

CXX		= g++ -std=c++2a  #-DNO_DEFAULT_MAPPING
INCLUDES	= -I $(FF_ROOT) 
CXXFLAGS  	= -Wall
LINKERFLAG = -lm

LDFLAGS 	= -pthread
OPTFLAGS	= -O3 -finline-functions -DNDEBUG

.PHONY: all clean cleanall
.SUFFIXES: .cpp 


ff:  ff.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDES} $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LINKERFLAG) $(ARGS)
seq: seq.cpp
	$(CXX) $(CXXFLAGS)  $(OPTFLAGS) -o $@ $<  $(LINKERFLAG) $(ARGS)
par: par.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LINKERFLAG) $(ARGS)
grp: grp.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LINKERFLAG)
