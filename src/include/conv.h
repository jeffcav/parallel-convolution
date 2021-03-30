#ifndef __CONV_H__
#define __CONV_H__

#include "image.h"
/** @brief Apply a nxn filter to an image. */
struct bmp_image *conv_2d(struct bmp_image *img, char *kernel, int n);

#endif
