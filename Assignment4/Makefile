.PHONY: clean debug run

CXX = gcc
CXXFLAGS = 
LDFLAGS =

SOURCES = wordgen.c wordsearch.c pager.c launcher.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = launcher

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

.c.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS) debug.exe *.stackdump *~ *.dSYM/

debug:
	$(CXX) $(CXXFLAGS) -g -o debug.exe $(SOURCES)

run:
	./$(EXECUTABLE)