CC=g++
CFLAGS=-c -g -Wall  `pkg-config --cflags opencv`
LIBS=-lm  `pkg-config --libs opencv`
LDFLAGS=
SOURCES=funddraw.cpp funddrawutils.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=funddraw



all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f *.o core *.core $(EXECUTABLE) *~

