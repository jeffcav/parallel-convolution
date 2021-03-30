#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/conv.h"
#include "include/bmp.h"

#define SRCFILE "images/lena.bmp"
#define COPYFILE "images/out/test_lena_copy.bmp"
#define CONVFILE "images/out/test_lena_conv.bmp"
#define LINESFILE "images/out/test_lena_lines.bmp"

void copy_file(const char *src, const char *dst) {
	struct bmp_image *img;

	img = bmp_load(src);
	bmp_describe(img);
	bmp_save(img, dst);
	bmp_destroy(img);

}

void draw_black_lines(const char *src, const char *dst, int nrows) {
	struct bmp_image *img;

	img = bmp_load(src);
	bmp_describe(img);

	memset(img->data, 0, img->header.width);

	bmp_save(img, dst);
	bmp_destroy(img);
}

void conv(const char *src, const char *dst) {
	char identity[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	int n = 3;
	struct bmp_image *img, *out;

	img = bmp_load(src);
	bmp_describe(img);

	out = conv_2d(img, identity, n);

	bmp_save(out, dst);

	bmp_destroy(img);
	bmp_destroy(out);
}

int main(int argc, char *argv[]) {
	copy_file(SRCFILE, COPYFILE);
	conv(SRCFILE, CONVFILE);
	draw_black_lines(SRCFILE, LINESFILE, 10);

	return 0;
}

