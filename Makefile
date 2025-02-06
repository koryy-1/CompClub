CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp fileReader.cpp parser.cpp trackingSystem.cpp console.cpp utils.cpp
OBJECTS=$(SOURCES:%.cpp=bin/%.o)
EXECUTABLE=bin/main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

bin/%.o: %.cpp | bin
	$(CC) $(CFLAGS) -o $@ -c $<

bin:
	mkdir -p bin

clean:
	rm -rf bin
