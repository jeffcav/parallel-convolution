#ifndef __CONV_H__
#define __CONV_H__

#include "image.h"

/** @brief Apply a nxn filter to an image. */
struct raw_image *conv_2d(struct raw_image *img, const char *kernel, int n);
struct raw_image *conv_2d_parallel(struct raw_image *in, const char *kernel, int n);

#endif
