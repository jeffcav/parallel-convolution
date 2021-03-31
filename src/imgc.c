#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/conv.h"
#include "include/image.h"

void make_bitmap(int w, int h, const char *filen) {
	int i;
	struct bmp_image *bmp;
	struct raw_image *img = raw_create(w, h, 1);

	srand(time(NULL));
	for (i = 0; i < w*h; i++) {
		img->data[i] = rand() % 256;
	}

	bmp = raw_to_bmp(img);
	bmp_save(bmp, filen);

	bmp_destroy(bmp);
	raw_destroy(img);
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		printf("Usage: imgc width heigh output_filename");
	}

	make_bitmap(atoi(argv[1]), atoi(argv[2]), argv[3]);

	return 0;
}

