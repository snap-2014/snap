all: stoke_dist.o normal.o exponential.o lognormal.o
	g++ -std=c++0x main.cc -o test stoke_dist.o normal.o exponential.o lognormal.o

stoke_dist.o: stoke_dist.cc
	g++ -std=c++0x -c stoke_dist.cc
normal.o: normal.s
	g++ -std=c++0x -c normal.s
exponential.o: exponential.s
	g++ -std=c++0x -c exponential.s
lognormal.o: lognormal.s
	g++ -std=c++0x -c lognormal.s

clean:
	rm -f *.o test
