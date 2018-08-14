CC=clang
CFLAGS=-g `llvm-config --cflags`
LD=clang++
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: main

main.o: main.c util.c
	$(CC) $(CFLAGS) -c main.c util.c

main: main.o util.o
	$(LD) main.o util.o $(LDFLAGS) -o $@

exec: main
	./main
	lli main.ll

clean:
	-rm -f main.o main main.bc main.ll util.o
