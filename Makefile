ifndef FF_ROOT
FF_ROOT		= $(HOME)/fastflow
endif

CXX		= g++-11 -std=c++2a  #-DNO_DEFAULT_MAPPING
INCLUDES	= -I $(FF_ROOT) 
CXXFLAGS  	= -Wall
LINKERFLAG = -lm

LDFLAGS 	= -pthread
OPTFLAGS	= -O3 -finline-functions -DNDEBUG

.PHONY: all clean cleanall
.SUFFIXES: .cpp 


%: %.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDES} $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LINKERFLAG)

ff: ff.cpp
seq: seq
par: par
grp: grp
