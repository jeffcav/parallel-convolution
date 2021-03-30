#include <stdlib.h>
#include <stdio.h>
#include "include/bmp.h"
#include "include/conv.h"

/**
 * @brief Apply a nxn filter to a nxn region of an image.
 */
static char conv2D_region(struct bmp_image *img, int offset, char *kernel, int n) {
	int i, j;
	unsigned char acc = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			acc += (img->data[offset + j] * kernel[(i*n)+j]);
		}
		offset += img->header.width;
	}

	return acc;
}

struct bmp_image *conv_2d(struct bmp_image *img, char *kernel, int n) {
	int row, col, diff_size;
	struct bmp_image *out;

	out = malloc(sizeof(struct bmp_image));
	out->header = img->header;
	out->header.width = img->header.width - (2 * (n/2));
	out->header.height = img->header.height - (2 * (n/2));
	diff_size = (img->header.width * img->header.height) - (out->header.width * out->header.height);
	out->header.size -= diff_size;
	out->data = malloc(out->header.size - sizeof(struct bmp_header));

	for (row = 0; row < out->header.width; row++) {
		for (col = 0; col < out->header.height; col++) {
			int in_offset, out_offset;

			in_offset = (row * img->header.width) + col;
			out_offset = (row * out->header.width) + col;

			out->data[out_offset] = conv2D_region(img, in_offset, kernel, n);
		}
	}

	return out;
}
