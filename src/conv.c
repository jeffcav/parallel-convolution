#include <stdlib.h>
#include "include/conv.h"

/**
 * @brief Apply a nxn filter to a nxn region of an image.
 */
static char conv2D_region(char **image, float **kernel, int n) {
	int i, j;
	unsigned char acc = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			acc += (image[i][j] * kernel[i][j]);
		}
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

char **conv2D(char **image, float **kernel, int n) {
	return NULL;
}
