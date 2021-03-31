#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/image.h"

void conv(const char *src, const char *dst) {
	char identity[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	int n = 3;
	struct bmp_image *img, *out;
	struct raw_image *rin, *rout;

	img = bmp_load(src);
	rin = bmp_to_raw(img, 0);
	rout = conv_2d(rin, identity, n);
	out = raw_to_bmp(rout);
	bmp_save(out, dst);

	bmp_destroy(img);
	bmp_destroy(out);
	raw_destroy(rin);
	raw_destroy(rout);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: serial src_file.bmp dst_file.bmp");
	}

	conv(argv[1], argv[2]);
}
