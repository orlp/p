CPPFLAGS=-std=c++11 -Wall -pedantic

all: p

%.o: %.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

p: src/main.o src/lexer.o src/parse.o
	g++ $(CFLAGS) -o p src/lexer.o src/parse.o src/main.o 

.PHONY: clean

clean:
	find . -type f -name "*.o" -delete
	rm p
