VPATH = .:utils/selfcalib
CC=g++
CFLAGS=-c -g -Wall  `pkg-config --cflags opencv` `gsl-config --cflags`
LIBS=-lm -lboost_filesystem `pkg-config --libs opencv` `gsl-config --libs`
LDFLAGS=
SOURCES=main.cpp argproc.cpp HRImage.cpp HRprimitives.cpp general.cpp pgmutils.cpp sift.cpp matching.cpp cvmodelest.cpp cvfundam.cpp visiongen.cpp utils/selfcalib/focallength.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sfmh1
PROJECT = libhrlib.a



all: $(EXECUTABLE) $(PROJECT)

lib: $(PROJECT)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)


$(PROJECT): $(OBJECTS)
	ar rcs  $(PROJECT) $(OBJECTS)


.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o core *.core $(EXECUTABLE) *~ $(PROJECT)

cleandirs: clean
	$(MAKE) -C utils/fundutils -f Makefile clean
	$(MAKE) -C utils/decompose -f Makefile clean
	$(MAKE) -C utils/selfcalib -f Makefile clean


builddirs: all
	cd "utils/fundutils" && $(MAKE) -f Makefile
	cd "utils/decompose" && $(MAKE) -f Makefile
	cd "utils/selfcalib" && $(MAKE) -f Makefile
