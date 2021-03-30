#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

char src_file[] = "images/lena.bmp";

#define SRCFILE "images/lena.bmp"
#define COPYFILE "images/lena_copy.bmp"
#define CONVFILE "images/lena_conv.bmp"
#define LINESFILE "images/lena_lines.bmp"

void draw_black_lines(const char *src, const char *dst, int nrows) {
	int i;
	struct bmp_image *img;

	img = bmp_load(src);
	bmp_describe(img);

	//for (i = 0; i < nrows; i++) {
	memset(img->data, 0, img->header.width);
	//}

	bmp_save(img, dst);
	bmp_destroy(img);
}

void conv(const char *src, const char *dst) {
	//float kernel[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
	char kernel[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	//char kernel[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
	int n = 3;
	struct bmp_image *img, *out;

	img = bmp_load(src);
	bmp_describe(img);

	out = conv_2d(img, kernel, n);

	bmp_save(out, dst);

	bmp_destroy(img);
	bmp_destroy(out);
}

void copy_file(const char *src, const char *dst) {
	struct bmp_image *img;

	img = bmp_load(src);
	bmp_describe(img);
	bmp_save(img, dst);
	bmp_destroy(img);

}

int main(int argc, char *argv[]) {
	copy_file(SRCFILE, COPYFILE);
	conv(SRCFILE, CONVFILE);
	draw_black_lines(SRCFILE, LINESFILE, 10);
	//unpadded();
	//padded();
	//same_conv();
}

