#ifndef __CONV_H__
#define __CONV_H__

/**
 * @brief Apply a nxn filter to an image.
 */
char **conv2D(char **image, float **kernel, int n);

#endif
