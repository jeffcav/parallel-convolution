#include <stdlib.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

int main(int argc, char *argv[]) {
	char *image;
	struct BMPHeader hdr;
	char filename[] = "images/lena_gray.bmp";

	image = load_bmp(filename, &hdr);

	printf("-- %s --\n", filename);
	print_bmp_header(&hdr);
	free(image);

	return 0;
}

