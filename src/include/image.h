#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <inttypes.h>

#pragma pack(1)
struct bmp_header {             // Total: 54 bytes
  uint16_t type;              // Magic identifier
  uint32_t size;              // File size in bytes
  uint16_t reserved1;         // Not used
  uint16_t reserved2;         // Not used
  uint32_t offset;            //
  uint32_t header_size;       // Header size in bytes
  uint32_t width;             // Width of the image
  uint32_t height;            // Height of image
  uint16_t planes;            // Number of color planes
  uint16_t bits;              // Bits per pixel
  uint32_t compression;       // Compression type
  uint32_t imagesize;         // Image size in bytes
  uint32_t xresolution;       // Pixels per meter
  uint32_t yresolution;       // Pixels per meter
  uint32_t num_colors;          // Number of colors
  uint32_t important_colors;  // Important colors
};

#pragma pack(1)
struct bmp_color_table {      // Total: 4 bytes
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t reserved;
};

struct bmp_image {
    struct bmp_header header;
    //struct bmp_color_table table[256];
    unsigned char table[1024];
    unsigned char* data;
};

struct raw_image {
  int width;
  int height;
  int nchannels;
  unsigned char *data;
};


/** @brief Prints the contents of a BMP header. */
void bmp_describe(const struct bmp_image *img);

/** @brief Loads a BMP file as an array. */
struct bmp_image *bmp_load(const char *filepath);

/** @brief Saves an array to a BMP file. */
int bmp_save(const struct bmp_image *img, const char *filepath);

void bmp_destroy(struct bmp_image *img);

void raw_destroy(struct raw_image *raw);

struct bmp_image *raw2bmp(struct raw_image *raw);

struct raw_image *bmp2raw(struct bmp_image *bmp);

#endif
