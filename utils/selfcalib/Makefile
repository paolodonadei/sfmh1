#LOCALLIBS=/home/houman/work/thesiscode/libraries/locallibs
LOCALLIBS=/home/houman/Desktop/code/locallibs
CC=g++
INC = -I../../ -I'$(LOCALLIBS)/include'
CFLAGS=$(INC) -c -g -Wall
LIBS= -llevmar  -lhrlib  `pkg-config opencv --cflags --libs` -lboost_filesystem -lboost_system -llapack -lblas -lf2c
LDFLAGS=-L../../ -L'$(LOCALLIBS)/lib'
SOURCES=selfcalib.cpp focallength.cpp nonlinSClvm.cpp ../fundutils/funddrawutils.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=intrinsic


all: $(EXECUTABLE)



$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)


.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f *.o core *.core $(EXECUTABLE) *~

