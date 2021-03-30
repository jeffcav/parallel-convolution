#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/conv.h"
#include "include/image.h"

#define SRCFILE "images/lena.bmp"
#define COPYFILE "images/out/test_lena_copy.bmp"
#define COPYRAWFILE "images/out/test_lena_copy_raw.bmp"
#define COPYPADDEDFILE "images/out/test_lena_copy_padded.bmp"
#define CONVFILE "images/out/test_lena_conv.bmp"
#define CONVPADDEDFILE "images/out/test_lena_conv_padded.bmp"
#define LINESFILE "images/out/test_lena_lines.bmp"

void copy_file(const char *src, const char *dst) {
	struct bmp_image *img;

	img = bmp_load(src);
	bmp_describe(img);
	bmp_save(img, dst);
	bmp_destroy(img);

}

void copy_file_with_raw(const char *src, const char *dst) {
	struct bmp_image *bmp1, *bmp2;
	struct raw_image *raw;

	bmp1 = bmp_load(src);
	raw = bmp_to_raw(bmp1, 0);
	bmp2 = raw_to_bmp(raw);

	bmp_describe(bmp2);
	bmp_save(bmp2, dst);

	bmp_destroy(bmp1);
	bmp_destroy(bmp2);
	raw_destroy(raw);

}

void copy_file_with_padding(const char *src, const char *dst) {
	struct bmp_image *bmp1, *bmp2;
	struct raw_image *raw;

	bmp1 = bmp_load(src);
	raw = bmp_to_raw(bmp1, 1);
	bmp2 = raw_to_bmp(raw);

	bmp_describe(bmp2);
	bmp_save(bmp2, dst);

	bmp_destroy(bmp1);
	bmp_destroy(bmp2);
	raw_destroy(raw);

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
	struct raw_image *rin, *rout;

	img = bmp_load(src);
	printf("conv: Describing original image:\n");
	bmp_describe(img);

	rin = bmp_to_raw(img, 0);
	rout = conv_2d_raw(rin, identity, n);

	out = raw_to_bmp(rout);
	bmp_save(out, dst);

	printf("conv: Describing convolved image:\n");
	bmp_describe(out);

	bmp_destroy(img);
	bmp_destroy(out);
	raw_destroy(rin);
	raw_destroy(rout);
}

void conv_padded(const char *src, const char *dst) {
	char identity[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	int n = 3;
	struct bmp_image *img, *out;
	struct raw_image *rin, *rout;

	img = bmp_load(src);
	printf("conv: Describing original image:\n");
	bmp_describe(img);

	rin = bmp_to_raw(img, 1);
	rout = conv_2d_raw(rin, identity, n);

	out = raw_to_bmp(rout);
	bmp_save(out, dst);

	printf("conv: Describing convolved image:\n");
	bmp_describe(out);

	bmp_destroy(img);
	bmp_destroy(out);
	raw_destroy(rin);
	raw_destroy(rout);
}

int main(int argc, char *argv[]) {
	copy_file(SRCFILE, COPYFILE);
	copy_file_with_raw(SRCFILE, COPYRAWFILE);
	copy_file_with_padding(SRCFILE, COPYPADDEDFILE);
	conv(SRCFILE, CONVFILE);
	conv_padded(SRCFILE, CONVPADDEDFILE);
	draw_black_lines(SRCFILE, LINESFILE, 10);

	return 0;
}

