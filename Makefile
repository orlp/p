CPPFLAGS=-std=c++11 -Wall -pedantic

all: p

%.o: %.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

p: src/main.o src/lexer.o src/ast.o
	g++ $(CFLAGS) -o p src/main.o src/lexer.o

.PHONY: clean

clean:
	find . -type f -name "*.o" -delete
	rm p
