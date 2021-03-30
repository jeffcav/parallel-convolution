LIBFILES = src/conv.c src/bmp.c

all: prepare test serial inspect parallel-mpi

prepare:
	mkdir build
	mkdir -p images/out

serial:
	gcc -Wall $(LIBFILES) src/serial.c -o build/serial

inspect:
	gcc -Wall $(LIBFILES) src/inspect.c -o build/inspect

test:
	gcc -Wall $(LIBFILES) src/test.c -o build/test

parallel-mpi:
	mpicc -Wall $(LIBFILES) src/parallel-mpi.c -o build/parallel-mpi

run-parallel-mpi:
	mpirun -n 4 ./build/parallel-mpi images/lena.bmp

run-serial:
	./build/serial

run-test:
	./build/test

clean:
	rm -rf build

