#include <stdlib.h>
#include <stdio.h>
#include "include/bmp.h"
#include "include/conv.h"

/**
 * @brief Apply a nxn filter to a nxn region of an image.
 */
static char conv2D_region_old(struct BMPImage *img, int offset, char *kernel, int n) {
	int i, j, step;
	unsigned char acc = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			acc += (img->data[offset + j] * kernel[(i*n)+j]);
		}
		offset += img->header.width_px;
	}

	return acc;
}

/**
 * @brief Apply a nxn filter to a nxn region of an image.
 */
static char conv2D_region(struct bmp_image *img, int offset, char *kernel, int n) {
	int i, j, step;
	unsigned char acc = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			acc += (img->data[offset + j] * kernel[(i*n)+j]);
		}
		offset += img->header.width;
	}

	return acc;
}


/**
 * @brief Points to @param steps pixels from the beggining of the image.
 *
 * Image is walked bottom-up, row by row.
 */
static char **walk(char **image, int row_padding, int steps) {
	return NULL;
}

/**
 * @brief Address of first data for a given convolution
 * @param img Image
 * @param n Dimension n of a nxn kernel
 *
 * TODO: support padding and stride
 */
static int conv2D_img_idx(struct BMPImage *img, int res_row, int res_col, int n) {
	return (res_row * img->header.width_px) + res_col;
}

int conv2D_old(struct BMPImage *img, char *kernel, int n, struct BMPImage *result) {
	int diff_size;
	int row, col, chan, num_rows, num_cols;
	int res_num_rows, res_num_cols;
	int i, offset;

	num_rows = img->header.width_px - (2 * (n/2));
	num_cols = img->header.height_px - (2 * (n/2));
	diff_size = (img->header.width_px * img->header.height_px);

	printf("num_res_cols=%d num_res_rows=%d\n\n", res_num_cols, res_num_rows);

	result->header = img->header;
	result->data = malloc(res_num_rows * res_num_cols);

	for (row = 0; row < res_num_rows; row++) {
		for (col = 0; col < res_num_cols; col++) {
			offset = (row * img->header.width_px) + col;

			//printf("row=%d col=%d offset=%d\n", row, col, offset);

			result->data[(row * res_num_cols) + col] = conv2D_region_old(img, offset, kernel, n);
		}
	}

	result->header.width_px -= (2 * (n / 2));
	result->header.height_px -= (2 * (n / 2));

	return 0;
}

struct bmp_image *conv_2d(struct bmp_image *img, char *kernel, int n) {
	int i, offset;
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
