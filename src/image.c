#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include "include/image.h"

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

int bmp_configure_header(struct bmp_image *img, int width, int height, int nchannels) {
    int padding, file_size;

    file_size = (width * height * nchannels) + sizeof(struct bmp_header) + sizeof(img->table);
    padding = (file_size & 0x3);
    if (padding)
        file_size += padding;

    img->header.type = 0x4d42;
    img->header.size = file_size;
    img->header.offset = sizeof(struct bmp_header) + sizeof(img->table);
    img->header.header_size = 0x28;
    img->header.width = width;
    img->header.height = height;
    img->header.planes = nchannels;
    img->header.bits = 8;
    img->header.compression = 0;
    img->header.imagesize = 0;
    img->header.xresolution = 0;
    img->header.yresolution = 0;
    img->header.num_colors = 0;
    img->header.important_colors = 0;

    return padding;
}

// TODO use a .h file with an array containing the color table
int bmp_configure_color_table(struct bmp_image *img) {
    FILE *fp;
    char filepath[] = "resources/color-table.bin";

    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        printf("[ERROR] Could not open %s\n", filepath);
        return -1;
    }

    if (fread(&img->table[0], 1024, 1, fp) != 1) {
        printf("[ERROR] Could not read color table from %s\n", filepath);
        return -1;
    }

    fclose(fp);
    return 0;
}

struct bmp_image *bmp_create(int width, int height, int nchannels) {
    int padding;
    struct bmp_image *img = calloc(sizeof(struct bmp_image), 1);

    padding = bmp_configure_header(img, width, height, nchannels);
    img->data = malloc((width * height * nchannels) + padding);
    if (bmp_configure_color_table(img)) {
        bmp_destroy(img);
        return NULL;
    }

    return img;
}

struct raw_image *raw_create(int width, int height, int nchannels) {
    struct raw_image *img = calloc(sizeof(struct raw_image), 1);

    img->data = malloc(width * height * nchannels);
    img->width = width;
    img->height = height;
    img->nchannels = nchannels;

    return img;
}

 struct bmp_image *bmp_load(const char *filepath) {
    FILE *fp;
    int data_size;
    struct bmp_image *img = NULL;

    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        printf("Could not open %s\n", filepath);
        return NULL;
    }

    img = malloc(sizeof(struct bmp_image));

    // read header (54 bytes)
    if (fread(&img->header, sizeof(struct bmp_header), 1, fp) != 1) {
        printf("Could not read header from %s\n", filepath);
        free(img);
        return NULL;
    }

    // read color table (1024 bytes in 8-bit bitmaps)
    if (fread(&img->table[0], sizeof(char), sizeof(img->table), fp) != sizeof(img->table)) {
        printf("Could not read color table from %s\n", filepath);
        free(img);
        return NULL;
    }

    data_size = img->header.size - img->header.offset;
    img->data = malloc(sizeof(unsigned char) * data_size);

    if (fread(img->data, sizeof(char), data_size, fp) != data_size) {
        printf("Failed reading image data from %s\n", filepath);
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

    if (fwrite(img->table, sizeof(char), sizeof(img->table), fp) != sizeof(img->table)) {
        printf("Could not write header in %s\n", filepath);
        fclose(fp);
        return 0;
    }

    data_size = img->header.size - img->header.offset;
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

void raw_destroy(struct raw_image *raw) {
    free(raw->data);
    free(raw);
}

struct bmp_image *raw2bmp(struct raw_image *raw) {
    int row, col;
    int raw_offset, bmp_offset;
    struct bmp_image *bmp = bmp_create(raw->width, raw->height, raw->nchannels);

    if (bmp == NULL)
        return NULL;

    for (row = 0; row < raw->height; row++) {
        raw_offset = (row * raw->width);
        bmp_offset = ((raw->height-1) * raw->width) - raw_offset;

        for (col = 0; col < raw->width; col++) {
            bmp->data[bmp_offset+col] = raw->data[raw_offset+col];
        }
    }

    return bmp;
}

struct raw_image *bmp2raw(struct bmp_image *bmp) {
    int row, col;
    int raw_offset, bmp_offset;
    struct raw_image *raw = malloc(sizeof(struct raw_image));

    raw->width = bmp->header.width;
    raw->height = bmp->header.height;
    raw->nchannels = bmp->header.planes;

    raw->data = malloc(raw->width * raw->height * raw->nchannels);

    for (row = 0; row < raw->height; row++) {
        raw_offset = (row * raw->width);
        bmp_offset = ((raw->height-1) * raw->width) - raw_offset;

        for (col = 0; col < raw->width; col++) {
            raw->data[raw_offset + col] = bmp->data[bmp_offset + col];
        }
    }

    return raw;
}
