LOCALLIBS=/home/houman/work/thesiscode/libraries/locallibs
VPATH = .:utils/selfcalib
INC = -I. -I'$(LOCALLIBS)'/include -Iutils/selfcalib  -Iutils/sbainterface/
CC=g++
CFLAGS=$(INC) -c -g -Wall
LIBS= -lm  -llevmar  -lhighgui -lsba -lcxcore -lcv -lboost_filesystem -lboost_system    -l5point -limage  -lmatrix -llapack   -lcblas -lblas  -lf2c  -lminpack `pkg-config opencv --cflags --libs` -lgfortran
LDFLAGS= -L'$(LOCALLIBS)'/lib -L/usr/lib
SOURCES=main.cpp argproc.cpp HRImage.cpp HRprimitives.cpp general.cpp pgmutils.cpp sift.cpp matching.cpp visiongen.cpp HRstructure.cpp
OBJECTS=$(SOURCES:.cpp=.o) utils/selfcalib/focallength.o utils/selfcalib/nonlinSClvm.o    utils/sbainterface/imgproj.o  utils/sbainterface/readparams.o  utils/sbainterface/eucsbademo.o
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
