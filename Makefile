LIBFILES = src/conv.c src/image.c

all: prepare test serial inspect parallel-mpi color-table imgc parallel-mpi-omp parallel-omp

prepare:
	mkdir build
	mkdir -p images/out

serial:
	gcc -Wall $(LIBFILES) src/serial.c -o build/serial -fopenmp

inspect:
	gcc -Wall $(LIBFILES) src/inspect.c -o build/inspect -fopenmp

imgc:
	gcc -Wall $(LIBFILES) src/imgc.c -o build/imgc -fopenmp

color-table:
	gcc -Wall $(LIBFILES) src/color-table.c -o build/color-table -fopenmp

test:
	gcc -Wall $(LIBFILES) src/test.c -o build/test -fopenmp

parallel-mpi:
	mpicc -Wall $(LIBFILES) src/parallel-mpi.c -o build/parallel-mpi -fopenmp

parallel-omp:
	mpicc -Wall $(LIBFILES) src/parallel-omp.c -o build/parallel-omp -fopenmp

parallel-mpi-omp:
	mpicc -Wall $(LIBFILES) src/parallel-mpi-omp.c -o build/parallel-mpi-omp -lm -fopenmp

run-parallel-mpi:
	mpirun -n 4 ./build/parallel-mpi images/lena.bmp

run-parallel-mpi-omp:
	mpirun -n 4 ./build/parallel-mpi images/lena.bmp

run-serial:
	./build/serial images/lena.bmp images/out/serial_lena.bmp

run-color-table:
	./build/color-table images/lena.bmp resources/color-table.bin

run-test:
	./build/test

clean:
	rm -rf build

