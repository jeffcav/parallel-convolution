#include <stdlib.h>
#include <stdio.h>
#include "include/conv.h"
#include "include/bmp.h"

int main(int argc, char *argv[]) {
	char **image;
	int height, width, channels;

	load_bmp("lena.bmp", image, &width, &height, &channels);

	return 0;
}

