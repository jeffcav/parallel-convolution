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
    int file_padding, file_size, row_padding;

    file_size = (width * height * nchannels) + sizeof(struct bmp_header) + sizeof(img->table);

    row_padding = (width & 0x3);
    if (row_padding)
        file_size += (height * row_padding);

    file_padding = (file_size & 0x3);
    if (file_padding)
        file_size += file_padding;

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

    return file_padding;
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
    int file_padding, row_padding;
    struct bmp_image *img = calloc(sizeof(struct bmp_image), 1);

    file_padding = bmp_configure_header(img, width, height, nchannels);
    row_padding = width & 0x3;

    img->data = malloc((width * height * nchannels) + (height * row_padding) + file_padding);
    if (bmp_configure_color_table(img)) {
        bmp_destroy(img);
        return NULL;
    }

    return img;
}

struct raw_image *raw_create(int width, int height, int nchannels) {
    struct raw_image *img = calloc(sizeof(struct raw_image), 1);

    img->data = calloc(width * height * nchannels, 1);
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

struct bmp_image *raw_to_bmp(struct raw_image *raw) {
    int row, col, row_padding;
    int raw_offset, bmp_offset;
    struct bmp_image *bmp = bmp_create(raw->width, raw->height, raw->nchannels);

    if (bmp == NULL)
        return NULL;

    row_padding = raw->width & 0x3;

    for (row = 0; row < raw->height; row++) {
        raw_offset = (row * raw->width);
        bmp_offset = ((raw->height-1) * (raw->width + row_padding)) - (row * (raw->width+row_padding));

        for (col = 0; col < raw->width; col++) {
            bmp->data[bmp_offset+col] = raw->data[raw_offset+col];
        }
    }

    return bmp;
}

struct raw_image *bmp_to_raw(struct bmp_image *bmp, int padding) {
    int row, col;
    struct raw_image *raw;
    int raw_offset, padded_offset, bmp_offset;

    raw = raw_create(bmp->header.width + (2*padding),
                     bmp->header.height + (2*padding),
                     bmp->header.planes);

    padded_offset = (padding * raw->width) + padding;
    for (row = 0; row < bmp->header.height; row++) {
        raw_offset = (row * bmp->header.width);
        bmp_offset = ((bmp->header.height-1) * bmp->header.width) - raw_offset;

        for (col = 0; col < bmp->header.width; col++) {
            raw->data[padded_offset + col] = bmp->data[bmp_offset + col];
        }
        padded_offset += raw->width;
    }

    return raw;
}
