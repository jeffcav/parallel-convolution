
all:
	mkdir build
	mpicc src/conv.c -o build/conv

run:
	mpiexec build/conv 2

clean:
	rm -rf build

