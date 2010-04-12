LOCALLIBS=/home/houman/work/thesiscode/libraries/locallibs/
VPATH = .:../..
CC=g++
INC = -I../../ -I$(LOCALLIBS)/include

CFLAGS=$(INC) -c -g -Wall   `gsl-config --cflags`
LIBS=-lm  -L$(LOCALLIBS)/lib -lhrlib -lboost_filesystem `gsl-config --libs`
LDFLAGS=-L../../
SOURCES=selfcalib.cpp focallength.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=intrinsic


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f *.o core *.core $(EXECUTABLE) *~

cleandirs: clean
	$(MAKE) -C utils/fundutils -f Makefile clean
	$(MAKE) -C utils/decompose -f Makefile clean
	$(MAKE) -C utils/selfcalib -f Makefile clean


builddirs: all
	cd "utils/fundutils" && $(MAKE) -f Makefile
	cd "utils/decompose" && $(MAKE) -f Makefile
	cd "utils/selfcalib" && $(MAKE) -f Makefile
