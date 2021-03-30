LIBFILES = src/conv.c src/image.c

all: prepare test serial inspect parallel-mpi color-table

prepare:
	mkdir build
	mkdir -p images/out

serial:
	gcc -Wall $(LIBFILES) src/serial.c -o build/serial

inspect:
	gcc -Wall $(LIBFILES) src/inspect.c -o build/inspect

color-table:
	gcc -Wall $(LIBFILES) src/color-table.c -o build/color-table

test:
	gcc -Wall $(LIBFILES) src/test.c -o build/test

parallel-mpi:
	mpicc -Wall $(LIBFILES) src/parallel-mpi.c -o build/parallel-mpi

run-parallel-mpi:
	mpirun -n 4 ./build/parallel-mpi images/lena.bmp

run-serial:
	./build/serial

run-color-table:
	./build/color-table images/lena.bmp resources/color-table.bin

run-test:
	./build/test

clean:
	rm -rf build

