CC=g++
CFLAGS=-c -g
LIBS=   -lwxmsw28_adv -lwxbase28_xml -lwxmsw28_core -lwxbase28_net -lwxbase28 -lwxmsw28_aui  -lwxmsw28_gl -lwxmsw28_html -lwxmsw28_media -lwxmsw28_richtext -lwxmsw28_xrc -lcomctl32 -lcomdlg32 -lwxpng  -lwxtiff -lwxzlib -lm -loledlg -lole32 -lolepro32 -loleacc -lwinspool -loleaut32 -lolecli32 -lolesvr32  -lkernel32 -lgdi32 -luuid -luser32 -lglut -lglu -lopengl32 -lgdi32 -lwinmm

LDFLAGS=
SOURCES=sfmARhr.cpp hrply.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=sfmarhr



all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f *.o core *.core *.exe $(EXECUTABLE) *~

