#include <inttypes.h>

struct BMPHeader {             // Total: 54 bytes
  uint16_t  type;             // Magic identifier: 0x4d42
  uint32_t  size;             // File size in bytes
  uint16_t  reserved1;        // Not used
  uint16_t  reserved2;        // Not used
  uint32_t  offset;           // Offset to image data in bytes from beginning of file (54 bytes)
  uint32_t  dib_header_size;  // DIB Header size in bytes (40 bytes)
  int32_t   width_px;         // Width of the image
  int32_t   height_px;        // Height of image
  uint16_t  num_planes;       // Number of color planes
  uint16_t  bits_per_pixel;   // Bits per pixel
  uint32_t  compression;      // Compression type
  uint32_t  image_size_bytes; // Image size in bytes
  int32_t   x_resolution_ppm; // Pixels per meter
  int32_t   y_resolution_ppm; // Pixels per meter
  uint32_t  num_colors;       // Number of colors
  uint32_t  important_colors; // Important colors
};

struct BMPImage {
    struct BMPHeader header;
    unsigned char* data;
};

/**
 * @brief Loads a BMP file to a 2D array.
 */
char *load_bmp(char *path, struct BMPHeader *hdr);

/**
 * @brief Saves a 2D array to a BMP file (.bmp).
 */
int save_bmp(char *path, char **image, int width, int heigh, int channels);

/**
 * @brief Prints the contents of a BMP header.
 */
void print_bmp_header(struct BMPHeader *hdr);
