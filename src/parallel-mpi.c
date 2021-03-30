#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include "include/conv.h"
#include "include/bmp.h"

#define SRCFILE "images/lena.bmp"
#define DSTFILE "images/out/serial_lena_conv.bmp"

/*
void conv(const char *src, const char *dst) {
	char identity_kernel[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	int n = 3;
	struct bmp_image *img, *out;

	img = bmp_load(src);
	out = conv_2d(img, identity, n);
	bmp_save(out, dst);

	bmp_destroy(img);
	bmp_destroy(out);
}
*/

void run_dispatcher(const char *filepath, int rank, int world_size) {
	struct bmp_image *img, *out, tmp;
	unsigned char *all_rows;
	unsigned char *img_rows;
	int rows_per_proc, num_cols;
	int kernel_dim = 3;
	char identity_kernel[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};

	if (rank == 0) {
		img = bmp_load(filepath);
		all_rows = img->data;
	}

	// TODO: support num rows not multiple of world_size
	/*
	rows_per_proc = img->header.height / world_size;
	if (img->header.height % world_size)
		fprintf(stderr, "[ERROR] number of rows not divisible by num procs\n");
	*/

	// TODO send rows per proc and num_cols at the very beggining
	// Assuming world_size=4 and a 512x512 image
	rows_per_proc = 128;
	num_cols = 512;

	img_rows = malloc(rows_per_proc * num_cols);

	MPI_Scatter(all_rows, rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, img_rows, rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);


	tmp.header.width = num_cols;
	tmp.header.height = rows_per_proc;
	tmp.header.planes = 1;
	tmp.header.size = (tmp.header.width * tmp.header.height * tmp.header.planes) + sizeof(struct bmp_header);
	tmp.data = img_rows;

	out = conv_2d(&tmp, identity_kernel, kernel_dim);

	if (rank == 0)
		bmp_describe(out);

	free(out);
	free(img_rows);
	if (rank == 0)
		bmp_destroy(img);
}

void run_worker() {

}

int main(int argc, char *argv[]) {
	int world_size, rank;

	if (argc != 2) {
    	fprintf(stderr, "Usage: parallel-mpi image.bmp\n");
    	exit(1);
  	}

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	run_dispatcher(argv[1], rank, world_size);
/*
	if (rank == 0) {
    	run_dispatcher();
  	} else {
		run_worker();
	}
*/
	MPI_Barrier(MPI_COMM_WORLD);
  	MPI_Finalize();

}
