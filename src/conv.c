#include <stdlib.h>
#include <stdio.h>
#include "include/image.h"
#include "include/conv.h"

/**
 * @brief Apply a nxn filter to a nxn region of an image.
 */
static char conv2D_region(struct raw_image *img, int offset, char *kernel, int n) {
	int i, j, k_offset = 0;
	unsigned char acc = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			acc += (img->data[offset + j] * kernel[k_offset + j]);
		}
		offset += img->width;
		k_offset += n;
	}

	return acc;
}

struct raw_image *conv_2d(struct raw_image *in, char *kernel, int n) {
	int row, col;
	struct raw_image *out;
	int in_offset, out_offset;

	out = raw_create(in->width - (2 * (n/2)),
					 in->height - (2 * (n/2)),
					 in->nchannels);

	in_offset = 0;
	out_offset = 0;

	for (row = 0; row < out->height; row++) {
		for (col = 0; col < out->width; col++) {
			out->data[out_offset + col] = conv2D_region(in, in_offset + col, kernel, n);
		}

		in_offset += in->width;
		out_offset += out->width;
	}

	return out;
}
