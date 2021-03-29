
all: prepare test serial inspect

prepare:
	mkdir build

serial:
	gcc src/conv.c src/bmp.c src/serial.c -o build/serial

inspect:
	gcc src/conv.c src/bmp.c src/inspect.c -o build/inspect

test:
	gcc src/conv.c src/bmp.c src/test.c -o build/test

parallel:
	#mpicc src/conv.c -o build/conv

run:
	mpiexec build/conv 2

clean:
	rm -rf build

