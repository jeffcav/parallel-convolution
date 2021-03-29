
all: prepare test serial

prepare:
	mkdir build

serial:
	#mpicc src/conv.c -o build/conv

test:
	gcc src/conv.c src/bmp.c src/test.c -o build/test

run:
	mpiexec build/conv 2

clean:
	rm -rf build

