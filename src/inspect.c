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
	img = bmp_load(src_file);

	printf("-- begin of %s --\n", src_file);
	bmp_describe(img);
	printf("-- end of %s --\n\n", src_file);
	bmp_destroy(img);
	return 0;
}

