#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include "include/bmp.h"

void print_bmp_header(struct BMPHeader *hdr) {
    printf("type: 0x%x\n", hdr->type);
    printf("size: 0x%x\n", hdr->size);
    printf("offset: 0x%x\n", hdr->offset);
    printf("DIB HDR size: 0x%x\n", hdr->dib_header_size);
    printf("width: 0x%x\n", hdr->width_px);
    printf("height: 0x%x\n", hdr->height_px);
    printf("num_planes: 0x%x\n", hdr->num_planes);
    printf("bits_per_pixel: 0x%x\n", hdr->bits_per_pixel);
    printf("compression: 0x%x\n", hdr->compression);
    printf("image_size_bytes: 0x%x\n", hdr->image_size_bytes);
    printf("x_resolution_ppm: 0x%x\n", hdr->x_resolution_ppm);
    printf("y_resolution_ppm: 0x%x\n", hdr->y_resolution_ppm);
    printf("num_colors: 0x%x\n", hdr->num_colors);
    printf("important_colors: 0x%x\n\n", hdr->important_colors);
}

static int load_bmp_header(FILE *fp, struct BMPHeader *hdr) {
    if (fread(&hdr->type, sizeof(hdr->type), 1, fp) < 1)
        return -1;

    if (fread(&hdr->size, sizeof(hdr->size), 1, fp) < 1)
        return -2;

    if (fread(&hdr->reserved1, sizeof(hdr->reserved1), 1, fp) < 1)
        return -3;

    if (fread(&hdr->reserved2, sizeof(hdr->reserved2), 1, fp) < 1)
        return -4;

    if (fread(&hdr->offset, sizeof(hdr->offset), 1, fp) < 1)
        return -5;

    if (fread(&hdr->dib_header_size, sizeof(hdr->dib_header_size), 1, fp) < 1)
        return -6;

    if (fread(&hdr->width_px, sizeof(hdr->width_px), 1, fp) < 1)
        return -7;

    if (fread(&hdr->height_px, sizeof(hdr->width_px), 1, fp) < 1)
        return -8;

    if (fread(&hdr->num_planes, sizeof(hdr->num_planes), 1, fp) < 1)
        return -9;

    if (fread(&hdr->bits_per_pixel, sizeof(hdr->bits_per_pixel), 1, fp) < 1)
        return -10;

    if (fread(&hdr->compression, sizeof(hdr->compression), 1, fp) < 1)
        return -11;

    if (fread(&hdr->image_size_bytes, sizeof(hdr->image_size_bytes), 1, fp) < 1)
        return -12;

    if (fread(&hdr->x_resolution_ppm, sizeof(hdr->x_resolution_ppm), 1, fp) < 1)
        return -13;

    if (fread(&hdr->y_resolution_ppm, sizeof(hdr->y_resolution_ppm), 1, fp) < 1)
        return -14;

    if (fread(&hdr->num_colors, sizeof(hdr->num_colors), 1, fp) < 1)
        return -15;

    if (fread(&hdr->important_colors, sizeof(hdr->important_colors), 1, fp) < 1)
        return -16;

    return 0;
}

static int save_bmp_header(FILE *fp, struct BMPHeader *hdr) {
    if (fwrite(&hdr->type, sizeof(hdr->type), 1, fp) < 1)
        return -1;

    if (fwrite(&hdr->size, sizeof(hdr->size), 1, fp) < 1)
        return -2;

    if (fwrite(&hdr->reserved1, sizeof(hdr->reserved1), 1, fp) < 1)
        return -3;

    if (fwrite(&hdr->reserved2, sizeof(hdr->reserved2), 1, fp) < 1)
        return -4;

    if (fwrite(&hdr->offset, sizeof(hdr->offset), 1, fp) < 1)
        return -5;

    if (fwrite(&hdr->dib_header_size, sizeof(hdr->dib_header_size), 1, fp) < 1)
        return -6;

    if (fwrite(&hdr->width_px, sizeof(hdr->width_px), 1, fp) < 1)
        return -7;

    if (fwrite(&hdr->height_px, sizeof(hdr->width_px), 1, fp) < 1)
        return -8;

    if (fwrite(&hdr->num_planes, sizeof(hdr->num_planes), 1, fp) < 1)
        return -9;

    if (fwrite(&hdr->bits_per_pixel, sizeof(hdr->bits_per_pixel), 1, fp) < 1)
        return -10;

    if (fwrite(&hdr->compression, sizeof(hdr->compression), 1, fp) < 1)
        return -11;

    if (fwrite(&hdr->image_size_bytes, sizeof(hdr->image_size_bytes), 1, fp) < 1)
        return -12;

    if (fwrite(&hdr->x_resolution_ppm, sizeof(hdr->x_resolution_ppm), 1, fp) < 1)
        return -13;

    if (fwrite(&hdr->y_resolution_ppm, sizeof(hdr->y_resolution_ppm), 1, fp) < 1)
        return -14;

    if (fwrite(&hdr->num_colors, sizeof(hdr->num_colors), 1, fp) < 1)
        return -15;

    if (fwrite(&hdr->important_colors, sizeof(hdr->important_colors), 1, fp) < 1)
        return -16;

    return 0;
}

static void load_bmp_data(FILE *fp, struct BMPHeader *hdr, char *image) {
    int bytes_read, row_size, row_padding_size, i;
    char *row, padding[4];

    row_size = hdr->width_px * hdr->num_planes;
    row_padding_size = row_size % 4;

    for (i = 0; i < hdr->height_px; i++) {
        bytes_read = fread(&image[i * row_size], row_size, 1, fp);

        if (row_padding_size)
            bytes_read = fread(padding, row_padding_size, 1, fp);
    }
}

static void save_bmp_data(FILE *fp, struct BMPHeader *hdr, char *image) {
    int bytes_written, row_size, row_padding_size, i;
    char *row, padding[4] = {0, 0, 0, 0};

    row_size = hdr->width_px * hdr->num_planes;
    row_padding_size = row_size % 4;

    for (i = 0; i < hdr->height_px; i++) {
        bytes_written = fwrite(&image[i * row_size], row_size, 1, fp);

        if (row_padding_size)
            bytes_written = fwrite(padding, row_padding_size, 1, fp);
    }
}

char *load_bmp(char *path, struct BMPHeader *hdr) {
    FILE *fp;
    char *image;
    int read_size, err;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("Could not open %s\n", path);
        return NULL;
    }

    err = load_bmp_header(fp, hdr);
    if (err) {
        printf("Error %d reading BMP header\n", err);
        return NULL;
    }

    image = malloc(hdr->width_px * hdr->height_px * hdr->num_planes);
    load_bmp_data(fp, hdr, image);

    fclose(fp);
    return image;
}

int save_bmp(char *path, struct BMPHeader *hdr, char *image) {
    FILE *fp;
    int read_size, err;

    fp = fopen(path, "wb");
    if (fp == NULL) {
        printf("Could not open %s\n", path);
        return -1;
    }

    err = save_bmp_header(fp, hdr);
    if (err) {
        printf("Error %d reading BMP header\n", err);
        return -2;
    }

    save_bmp_data(fp, hdr, image);

    fclose(fp);
    return 0;
}
