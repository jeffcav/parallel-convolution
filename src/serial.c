#include <stdlib.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

char src_file[] = "images/lena.bmp";

#define SRCFILE "images/lena.bmp"
#define COPYFILE "images/lena_copy.bmp"
#define CONVFILE "images/lena_conv.bmp"

/*
void padded() {
	int err;
	struct BMPImage img, output;
	char dst_file[] = "images/lena_padded.bmp";

	load_padded_bmp(src_file, &img, 1);

	print_bmp_header(&img.header);

	img.header.image_size_bytes = 0;
	err = save_bmp(dst_file, &img);
	if (err)
		printf("Could not save BMP: %d\n", err);
}

void unpadded() {
	int err;
	struct BMPImage img, output;
	char dst_file[] = "images/lena_copy.bmp";

	load_bmp(src_file, &img);

	print_bmp_header(&img.header);

	//img.header.image_size_bytes = 0;
	err = save_bmp(dst_file, &img);
	if (err)
		printf("Could not save BMP: %d\n", err);
}

void same_conv() {
	int err;
	struct BMPImage img, output;
	char dst_file[] = "images/lena_conv.bmp";
	//float kernel[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
	char kernel[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	//char kernel[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
	int n = 3;

	load_bmp(src_file, &img);

	print_bmp_header(&img.header);

	conv2D(&img, kernel, n, &output);

	output.header.image_size_bytes = 0;
	err = save_bmp(dst_file, &output);
	if (err)
		printf("Could not save BMP: %d\n", err);
}
*/

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
	//unpadded();
	//padded();
	//same_conv();
}

