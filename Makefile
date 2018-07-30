CC=clang
CFLAGS=-g `llvm-config --cflags`
LD=clang++
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: main

main.o: main.c
	$(CC) $(CFLAGS) -c $<

main: main.o
	$(LD) $< $(LDFLAGS) -o $@

main.bc: main
	./main

main.ll: main.bc
	llvm-dis $<

clean:
	-rm -f main.o main main.bc main.ll
