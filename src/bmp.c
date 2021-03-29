#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include "include/bmp.h"

void bmp_describe(const struct bmp_image *img) {
    printf("type: 0x%x\n",              img->header.type);
    printf("size: 0x%x\n",              img->header.size);
    printf("offset: 0x%x\n",            img->header.offset);
    printf("DIB HDR size: 0x%x\n",      img->header.header_size);
    printf("width: 0x%x\n",             img->header.width);
    printf("height: 0x%x\n",            img->header.height);
    printf("num_planes: 0x%x\n",        img->header.planes);
    printf("bits_per_pixel: 0x%x\n",    img->header.bits);
    printf("compression: 0x%x\n",       img->header.compression);
    printf("image_size_bytes: 0x%x\n",  img->header.imagesize);
    printf("x_resolution_ppm: 0x%x\n",  img->header.xresolution);
    printf("y_resolution_ppm: 0x%x\n",  img->header.yresolution);
    printf("num_colors: 0x%x\n",        img->header.num_colors);
    printf("important_colors: 0x%x\n\n",img->header.important_colors);
}

/*
static void load_padded_bmp_data(FILE *fp, struct BMPImage *img, int padding) {
    int bytes_read, row_size, row_bmp_padding_size, i, offset, step;
    char *row, bmp_padding[4];

    row_size = img->header.width_px * img->header.num_planes;
    row_bmp_padding_size = row_size % 4;

    step = row_size + (2 * padding);
    if (padding)
        offset = padding * step + 1;
    else
        offset = 0;

    for (i = 0; i < img->header.height_px; i++) {
        printf("Load offset = %d, step = %d\n", offset, step);

        bytes_read = fread(&img->data[offset], row_size, 1, fp);

        if (row_bmp_padding_size) {
            printf("Reading %d bytes of padding\n", row_bmp_padding_size);

            bytes_read = fread(bmp_padding, row_bmp_padding_size, 1, fp);
        }

        offset += step;
    }
}
*/

 struct bmp_image *bmp_load(const char *filepath) {
    FILE *fp;
    char onebyte;
    int data_size;
    struct bmp_image *img = NULL;

    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        printf("Could not open %s\n", filepath);
        return NULL;
    }

    img = malloc(sizeof(struct bmp_image));

    if (fread(&img->header, sizeof(struct bmp_header), 1, fp) != 1) {
        printf("Could not read header from %s\n", filepath);
        free(img);
        return NULL;
    }

    data_size = img->header.size - sizeof(struct bmp_header);
    img->data = malloc(sizeof(unsigned char) * data_size);

    if (fread(img->data, sizeof(char), data_size, fp) != data_size) {
        printf("Failed reading image data from %s\n", filepath);
        free(img->data);
        free(img);
        return NULL;
    }

    if (fread(&onebyte, sizeof(char), 1, fp) != 0) {
        printf("Failed reading image from %s: EOF not reached\n", filepath);
        free(img->data);
        free(img);
        return NULL;
    }

    fclose(fp);
    return img;
}

int bmp_save(const struct bmp_image *img, const char *filepath) {
    FILE *fp = NULL;
    int data_size;

    fp = fopen(filepath, "wb");
    if (fp == NULL) {
        printf("Could not open %s\n", filepath);
        return 0;
    }

    // write the header first
    if (fwrite(&(img->header), sizeof(struct bmp_header), 1, fp) != 1) {
        printf("Could not write header in %s\n", filepath);
        fclose(fp);
        return 0;
    }

    data_size = img->header.size - sizeof(struct bmp_header);
    if (fwrite(img->data, sizeof(char), data_size, fp) != data_size) {
        printf("Could not write image data in %s\n", filepath);
        fclose(fp);
        return 0;
    }

    fclose (fp);
    return 1;
}

void bmp_destroy(struct bmp_image *img) {
    free(img->data);
    free(img);
}

int load_padded_bmp(char *path, struct BMPImage *img, int padding) {
/*
    FILE *fp;
    int read_size, num_pads, err;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("Could not open %s\n", path);
        return -1;
    }

    err = load_bmp_header(fp, &img->header);
    if (err) {
        printf("Error %d reading BMP header\n", err);
        return -2;
    }

    num_pads = ((2 * img->header.width_px) + (4 * padding)) + (2 * img->header.height_px);
    printf("Size: %d\n", (img->header.width_px * img->header.height_px * img->header.num_planes) + num_pads);

    img->data = calloc((img->header.width_px * img->header.height_px * img->header.num_planes) + num_pads, 1);
    load_padded_bmp_data(fp, img, padding);

    img->header.width_px += (padding*2);
    img->header.height_px += (padding*2);

    printf("Size=%d W=%d H=%d Paddings=%d\n\n", (img->header.width_px * img->header.height_px * img->header.num_planes), img->header.width_px, img->header.height_px, num_pads);

    fclose(fp);
*/
    return 0;
}

