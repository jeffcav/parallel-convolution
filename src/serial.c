#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

#define SRCFILE "images/lena.bmp"
#define DSTFILE "images/out/serial_lena_conv.bmp"

void conv(const char *src, const char *dst) {
	char identity[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	int n = 3;
	struct bmp_image *img, *out;

	img = bmp_load(src);
	out = conv_2d(img, identity, n);
	bmp_save(out, dst);

	bmp_destroy(img);
	bmp_destroy(out);
}

int main(int argc, char *argv[]) {
	conv(SRCFILE, DSTFILE);
}
