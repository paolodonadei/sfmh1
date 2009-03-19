CC=g++
CFLAGS=-c -g -Wall  `pkg-config --cflags opencv`
LIBS=-lm -lboost_filesystem `pkg-config --libs opencv`
LDFLAGS=
SOURCES=main.cpp argproc.cpp HRImage.cpp HRprimitives.cpp general.cpp pgmutils.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sfmh1



all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f *.o core *.core $(EXECUTABLE) *~

