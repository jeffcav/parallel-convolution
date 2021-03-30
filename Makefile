LIBFILES = src/conv.c src/bmp.c

all: prepare test serial inspect

prepare:
	mkdir build
	mkdir -p images/out

serial:
	gcc -Wall $(LIBFILES) src/serial.c -o build/serial

inspect:
	gcc -Wall $(LIBFILES) src/inspect.c -o build/inspect

test:
	gcc -Wall $(LIBFILES) src/test.c -o build/test

parallel:
	# mpicc src/conv.c -o build/conv

run:
	# mpiexec build/conv 2

run-serial:
	./build/serial

run-test:
	./build/test

clean:
	rm -rf build

