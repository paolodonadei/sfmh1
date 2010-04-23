LOCALLIBS=/home/houman/work/thesiscode/libraries/locallibs
VPATH = .:utils/selfcalib
INC = -I. -I'$(LOCALLIBS)'/include -Iutils/selfcalib
CC=g++
CFLAGS=$(INC) -c -g -Wall
LIBS=   -lm -lgtk-x11-2.0 -lpthread -lgthread-2.0 -lrt -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lglib-2.0 -lgtk -lgtk -lgdk   -rdynamic -lgmodule -lglib -ldl -lXi -lXext -lX11 -lm  -llevmar  -lcxcore -lcv  -lhighgui  -lboost_filesystem -lboost_system  -ljasper -ljpeg -lpng -ltiff   -l5point -limage  -lmatrix -llapack   -lcblas -lblas  -lf2c  -lminpack
LDFLAGS= -L'$(LOCALLIBS)'/lib -L/usr/lib
SOURCES=main.cpp argproc.cpp HRImage.cpp HRprimitives.cpp general.cpp pgmutils.cpp sift.cpp matching.cpp visiongen.cpp HRstructure.cpp
OBJECTS=$(SOURCES:.cpp=.o) utils/selfcalib/focallength.o utils/selfcalib/nonlinSClvm.o
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
