CC=g++
INC = -I../../
CFLAGS=$(INC) -c -g -Wall  `pkg-config --cflags opencv`
LIBS=-lm  `pkg-config --libs opencv` -lhrlib -lboost_filesystem
LDFLAGS=-L../../
SOURCES=decomp.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=decomp


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f *.o core *.core $(EXECUTABLE) *~

