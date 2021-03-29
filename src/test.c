#include <stdlib.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

int main(int argc, char *argv[]) {
	int err;
	char *image;
	struct BMPHeader hdr;
	char filename[] = "images/lena_gray.bmp";

	char copy_filename[] = "images/lena_gray2.bmp";

	image = load_bmp(filename, &hdr);

	printf("-- %s --\n", filename);
	print_bmp_header(&hdr);

	err = save_bmp(copy_filename, &hdr, image);
	if (err)
		printf("Could not save BMP: %d\n", err);

	free(image);

	return 0;
}

