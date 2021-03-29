#include <stdlib.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

int main(int argc, char *argv[]) {
	int err;
	char *image;
	struct BMPImage img;
	char src_file[] = "images/lena_gray.bmp";
	char dst_file[] = "images/lena_gray2.bmp";

	load_bmp(src_file, &img);

	printf("-- %s --\n", src_file);
	print_bmp_header(&img.header);

	err = save_bmp(dst_file, &img);
	if (err)
		printf("Could not save BMP: %d\n", err);

	free(img.data);

	return 0;
}

