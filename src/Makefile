CC=clang
CFLAGS=-g `llvm-config --cflags`
LD=clang++
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: main

main.o: main.c util.c cmp.c
	$(CC) $(CFLAGS) -c main.c util.c cmp.c

main: main.o util.o cmp.o
	$(LD) main.o util.o cmp.o $(LDFLAGS) -o $@

exec: main
	./main
	lli main.ll

clean:
	-rm -f *.o main main.ll
