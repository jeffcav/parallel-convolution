#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/image.h"

int main(int argc, char *argv[]) {
	FILE *fp;
	struct bmp_image *img;
	char *src_file, *dst_file;

	if (argc < 3) {
		printf("Usage: inspect src_img dst_file\n");
		return 0;
	}

	src_file = argv[1];
	dst_file = argv[2];
	img = bmp_load(src_file);

	fp = fopen(dst_file, "wb");
    if (fp == NULL) {
        printf("Could not open %s\n", dst_file);
        return -1;
    }

	if (fwrite(&img->table[0], sizeof(img->table), 1, fp) != 1) {
		printf("Could not write color table to %s\n", dst_file);
		bmp_destroy(img);
		return -1;
	}

	printf("Color table sucessfully exported to %s.\n", dst_file);

	fclose(fp);
	bmp_destroy(img);
	return 0;
}

