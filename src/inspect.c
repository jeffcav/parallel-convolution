#include <stdlib.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

int main(int argc, char *argv[]) {
	struct bmp_image *img;
	char *src_file;

	if (argc < 2) {
		printf("Usage: inspect filepath.bmp\n");
		return 0;
	}

	src_file = argv[1];
	bmp_load(src_file);

	printf("-- %s --\n", src_file);
	bmp_describe(img);

	bmp_destroy(img);
	return 0;
}

